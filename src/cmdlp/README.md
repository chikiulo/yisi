# cmdlp
A template-based command line parser for c++.

## Philosophy
This is a fairly idea driven library, so you might find yourself struggling against it if you don't understand and/or agree with the guiding philosophy.

A computer program can be viewed as a function: it takes some inputs and produce some outputs. Most would agree that the typical terminal command takes some input (files) and produces some output (files). For example, `grep` takes some input file(s), and a search string, and outputs the matching lines to standard output. However, the command `grep` is actually not a single function, but a *family of functions* that differ slightly. The reason we call it a family of functions is that each individual member of that family can be uniquely described through a set of *hyper parameters*. In the case of `grep`, these are things like case-sensitivity and which type of pattern to use for matching. You can think of `grep` (or indeed any program) as a *very* advanced instance of a mathematical function. Logarithm, for example, has a hyper parameter *base*, a single input *x*, and a single output *y*.

&lt;opinion&gt;Thinking of a program in terms of a function with hyper parameters and inputs helps you write cleaner, clearer, and correcter code&lt;/opinion&gt;. This library has been designed to help with this. The hyper parameters are thought of as a set of *knobs* and *switches* that you use to set the value of a corresponding set of variables in your program. The inputs are though of as a set of files that your program operates on. (Don't worry, if it makes sense, there is nothing stopping you from turning a knob to set an input parameter.)

Obviously, the [POSIX standard](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html#tag_12_01) should be followed.

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
When the value being set is a container (eg. `vector`, `set`, or `unordered_map`), instead of overwriting the corresponding value, values are *inserted* every time that flag is given. You can still provide `fallback` content that will only be inserted if nothing is provided. Unlike value knobs it is not mandatory to add anything &ndash; an empty container is a perfectly valid value.

**Key-value storage as container**
When the container is a `map`, the value is considered to be a key&ndash;value pair, which you can give as either `key:value` or `key=value`. The keys need to be quoted if they contain spaces, colons or equal signs. Currently, there may be no spacing between the colon/equal sign and the key or the value.

**Categorial knobs**
Often time it is desirable to have a knob with discrete categorial values (like `enum`s) rather than numerical values (like `int`s or `float`s). In addition to knobs supporting `enum`s, this library comes with an XSLT style sheet that will convert a straight forward XML declaration of categorial values into a header containing a class that is specifically tailored to be used as the value of a categorial knob (see `data/magic_enum.xml` for an example of the declaration and `Makefile` for an example of how to generate code from it).

### Switches
Switches are created with `make_onswitch` and `make_offswitch`. An on-switch starts as `false`, but becomes `true` if it is set (regardless of how many times). And off-switch starts as `true`, but becomes `false` if it is set (regardless of how many times). This behavior differs between the command line and config files, with config files overriding the value of the underlying Boolean with a specific truth value (eg. `help=yes`).

### Config files
A config file is a text file that sets the parameters to your program much like variables in bash. Each line contains a *long* flag name followed by an equal sign, followed by the value to assign to the parameter. For example:
```
alpha=10
smooth=no
```
Notice that switches are set rather than flipped from an assumed current position. For containers as values, you can either give the name on multiple rows, give multiple values  on a single line, or any mix of the two strategies (there is no way to &ldquo;reset&rdquo; the container).

### Other files
Sometimes it is useful to have a whole file be part of the parameters to your program. In the `grep` example, `grep` allows you to give a file containing patterns to match, which is extremely useful every now and then. It may also be useful to be able to separate output more finely than is possible with standard out/error/log.

This library currently offers four helpful values that you can use to create knobs: `ifile`, `ofile`, `optional_ofile` and `ifile_prefix`. Why no `optional_ifile`? Let me refer you to the Philosophy section above.

The respective x`file`s manage the underlying files RAII-style, so files are opened during parameter validation, and closed when the managing object goes out of scope and is destroyed. If this is not what you want, just read in a string and implement your own file lifetime logic, this caters to the 90% case.

Example:
```
ifile poem;
optional_ofile verdict;
// ...
// initialize poem and verdict from the command line.
// ...
size_t lines = 0;
for (string line; getline(*poem, line); ++lines);
if (verdict) {
  if (lines == 3) {
    *verdict << "Did you write a haiku? Nice!" << endl;
  } else {
    *verdict << "Nice poem!" << endl;
  }
}
```

**`ifile`**
This represents a file that we will read from. You give either a filename or a dash for standard input, and the value functions as a smart pointer to an input stream (dereferences to an `istream&`). If a file that cannot be opened for input is given, parameter validation fails.

**(`optional_`)`ofile`**
This represents a file that we will write to. You give either a filename or a dash for standard output, and the value functions as a smart pointer to an input stream (dereferences to an `ostream&`). If a file that cannot be opened for output is given, parameter validation fails. In addition, `optional_ofile` can be cast to a `bool` to determine whether it was given or not; giving an invalid file fails validation, but giving no file does not.

**`ifile_prefix`**
This represents a collection of files with a common prefix that we will read from. It expands into a range of `ifile`s sharing the given prefix. Since there seems to still be some issues with using the C++17 filesystem header in a non-hacky way, this feature requires a POSIX-compatible filesystem &ndash; if you build it on a non-POSIX-compatible filesystem, using `ifile_prefix`es gives a compilation error.


### Diving deeper
Here are some advanced features that you might want to use after a while.

**More informative help screen**
In addition to the `argc` and `argv` parameters, you can also pass in a configuration object when creating your options. This allows you to provide strings describing your expected positional input as well as text that go before and after the list of options. Feed free to add newline characters which will be rendered as paragraph breaks.

**Pretty printing paragraphs**
The method for pretty printing paragraphs is completely generic and contained in the `paragraph.hpp` header. It provides a modifier for a stream that changes the behavior of the stream to print paragraphs instead of lines while the modifier is in scope. Feel free to use it elsewhere!

Example:
```
{
  // cout will print paragraphs (72 character lines with a 4 sapce
  // left margin and a 2 space paragraph break indentation)
  // as long as p is in scope.
  const auto p = paragraph(std::cout, 72, 4, 2);
  // write to cout just like you would!
  std::cout << "My first long paragraph." << std::endl;
  std::cout << "My second, even longer, paragraph." << std::endl;
}
// std::cout no longer prints paragraphs since p went out of scope.
```


### Advanced usage
There are some advanced features that you probably do not need.

**Custom readers**
You can use `on_read` with any knob or switch to provide a custom reader function. The signature should be `void(Value&, const char*)` where `Value` is the type you want to read into. It is also possible to specialize the struct template `com::masaers::cmdlp::from_cstr<Value>` with the desired `operator()`.

**Custom validators**
You can use `validate` with any knob to provide a custom validation function. The signature should be `bool(Value&)`, where `Value` is the type you want to validate. Note that you are allowed to change the value in this function, so beware that this constitutes the proverbial rope that you may end up shooting yourself in the foot with.

**Flip-flop switch-swotch**
It is possible to create a switch that moves every time it is given. They are created with the `make_switch` function, but is probably not what you want.
