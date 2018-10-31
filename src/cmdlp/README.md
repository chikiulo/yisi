# cmdlp
A template-based command line parser for c++.

## Philosophy
This is a fairly idea driven library, so you might find yourself struggling against it if you don't understand and/or agree with the guiding philosophy.

A computer program can be viewed as a function: it takes some inputs and produce some outputs. Most would agree that the typical terminal command takes some input (files) and produces some output (files). For example, `grep` takes some input file(s), and a search string, and outputs the matching lines to standard output. However, the command `grep` is actually not a single function, but a *family of functions* that differ slightly. The reason we call it a family of functions is that each individual member of that family can be uniquely described through a set of *hyper parameters*. In the case of `grep`, these are things like case-sensitivity and which type of pattern to use for matching. You can think of `grep` (or indeed any program) as a *very* advanced instance of a mathematical function. Logarithm, for example, has a hyper parameter *base*, a single input *x*, and a single output *y*.

&lt;opinion&gt;Thinking of a program in terms of a function with hyper parameters and inputs helps you write cleaner, clearer, and correcter code&lt;/opinion&gt;. This library has been designed to help. The hyper parameters are thought of as a set of *knobs* and *switches* that you use to set the value of a corresponding set of variables in your program. The inputs are though of as a set of files that your program operates on. (Don't worry, if it makes sense, there is nothing stopping you from turning a knob to set an input parameter.)

Obviously, the POSIX standard should be followed.

### But what about randomization?
Hate to break it to you, but your computer is incapable of randomness. What you are doing is implicitly setting the random seed of a pseudo random number generator typically based on the time of execution. *If you are a scientist, for the love of all that is reproducible, provide a knob to deterministically set the random seed!* If your result tables cannot be recreated reproducibility is out the window. Don't make wall clock at runtime an input to your program. (For funsies you may add a switch to explicitly disable reproducibility, if that's your thing.)

### In practice
In practice, this philosophy means that you already have a set of variables in your main function that needs to be given values; this is where the command line parser comes in. To separate the knobs-and-switches variables from other variables, you are encouraged to collect them in a class, and provide a method for the command-line parser to be initialized. The library makes it easy to initialize an object of such a class and keep it `const` after.

## Quick start
Create a `class` or `struct` with your parameters as values. It should also have an `init` function that adds the parameters to a `parser`.
```
struct my_params {
  int alpha;
  bool smooth;
  void init(com::masaers::cmdlp::parser& p) {
    using namespace com::masaers::cmdlp;
    p.add(make_knob(alpha))
      .desc("The amount of alpha.")
      .name('a', "alpha")
      ;
    p.add(make_onswitch(smooth))
      .desc("Perform smoothing.")
      .name("smooth")
      ;
  }
};
```
Then, in your `main` method, create an `options` object parameterized with `my_params`, and pass in `argc` and `argv`. If it evaluates to `false`, you should exit with a suggested exit code, otherwise your parameters have been correctly initialized, and your program is ready to deterministically do its job. The input files? They are stored as a `const vector<string>` that you access with the `args` member function.
```
int main(const int argc, const char** argv) {
  com::masaers::cmdlp::options<my_params> o(argc, argv);
  if (! o) {
    return o.exit_code();
  }
  for (const std::string& input_file_name : o.args()) {
    // Do your thing!
  }
}
```
Other than the flags you specified, your program now also has: `--config` that allows you to load parameters from a file, `-h` and `--help` that print a help section using the provided descriptions, and `--dumpto` which takes a filename (or `-` for standard output) and prints the parameters to a file that can later be used as an argument to `--config` to recreate the exact same behavior.

## Reference
Knobs and switches can be given names and descriptions using the `name` and `desc` methods. A name can be either a character (a short flag) or a string (a long flag). Short flags are used with a single `-`, and long flags with `--` (`-h` or `--help`). If you call `name` with both a short and long flag, the program will attempt to display them unified in the help printout (eg. `-[-h]elp`).

### Knobs
Knobs are created with `make_knob`. In addition to the `name` and `desc` methods, they also provide the `fallback` method, which specifies how to initialize the value if it is not provided. This is the only way to make knobs that are not mandatory. 

**Containers as values**
When the value being set is a container (eg. `vector`, `set`, or `unordered_map`), instead of overwriting the corresponding value, values are *inserted* every time that flag is given. You can still provide `fallback` content that will only be inserted if nothing is provided. Unlike value knobs it is not mandatory to add anything, an empty container is a perfectly valid value.

**Key-value storage as container**
When the container is a `map`, the value is considered to be a key&ndash;value pair, which you can give as either `key:value` or `key=value`.

### Switches
Switches are created with `make_onswitch` and `make_offswitch`. An on-switch starts as `false`, but becomes `true` if it is set (regardless of how many times). And off-switch starts as `true`, but becomes `false` if it is set (regardless of how many times). This behavior differs between the command line and config files, with config files overriding the value with of the underlying Boolean with a specific truth value (eg. `help=yes`).

### Config files
A config file is a text file that sets the parameters to your program much like variables in bash. Each line contains a *long* flag name followed by an equal sign, followed by the value to assign to the parameter. For example:
```
alpha=10
smooth=no
```
Notice that switches are set rather than flipped from an assumed current position. For containers as values, you can either give the name on multiple rows, give multiple values  on a single line, or any mix of the two strategies.

### Advanced usage
There are some advanced features that you probably do not need.

**Custom readers**
You can use `on_read` with any knob or switch to provide a custom reader function. The signature should be `void(Value&, const char*)` where `Value` is the type you want to read into. It is also possible to specialize the struct template `com::masaers::cmdlp::from_cstr<Value>` with the desired `operator()`.

**Flip-flop switch-swotch**
It is possible to create a switch that moves every time it is given. They are created with the `make_switch` function, but is probably not what you want.
