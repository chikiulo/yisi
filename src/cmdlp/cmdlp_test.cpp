/**
   Unit test for the command-line option library, which was cloned from:
   https://github.com/masaers/cmdlp (v0.4.1 tag)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */

#include "options.h"
#include "iooption.h"
#include "magic_enum.h"
#include "paragraph.h"
#include <iostream>
#include <vector>
#include <set>


/**
  Cryptic read function.
*/
void robber_lang(std::string& to, const char* from) {
  for (const char* c = from; *c != '\0'; ++c) {
    switch (*c) {
      case 'B': case 'C': case 'D': case 'F': case 'G': case 'H': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R':
      case 'S': case 'T': case 'V': case 'W': case 'X': case 'Y': case 'Z':
      to.push_back(*c);
      to.push_back('O');
      to.push_back(*c);
      break;
      case 'b': case 'c': case 'd': case 'f': case 'g': case 'h': case 'j':
      case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r':
      case 's': case 't': case 'v': case 'w': case 'x': case 'y': case 'z':
      to.push_back(*c);
      to.push_back('o');
      to.push_back(*c);
      break;
      default:
      to.push_back(*c);
    }
  }
}

struct local_options {
  int alpha;
  int beta;
  bool flip;
  bool on;
  bool off;
  std::set<std::string> strings;
  std::vector<std::string> cipher;
  std::map<std::string, float> constants;
  knobs::magic_level::value magic_level;
  com::masaers::cmdlp::ifile settings;
  com::masaers::cmdlp::ifile_prefix reference_files;
  // const char* cstr;
  void init(com::masaers::cmdlp::parser& p) {
    using namespace com::masaers::cmdlp;
    p.add(make_knob(alpha))
    .desc("The alpha value.")
    .name('a', "alpha")
    .name("ALPHA")
    .fallback(10)
    ;
    p.add(make_knob(beta))
    .desc("The beta value.")
    .name('b', "beta")
    ;
    p.add(make_switch(flip)).desc("A switch").name('f').name("flip");
    p.add(make_onswitch(on)).desc("Turns on").name("on");
    p.add(make_offswitch(off)).desc("Turns off").name("off");
    p.add(make_knob(strings))
    .desc("Some input strings")
    .name('s', "str")
    ;
    // p.add(make_knob(cstr)).desc("C-strings are bad, mmmkay.").name("cstr");
    p.add(make_knob(cipher))
    .desc("Encrypts the provided strings.")
    .name("cipher")
    .on_read(robber_lang)
    ;
    p.add(make_knob(constants))
    .desc("Named numeric constants.")
    .name('c', "const")
    .fallback({ {"pi", 3.14} })
    ;
    p.add(make_knob(magic_level))
    .desc(knobs::magic_level::desc())
    .name('m', "magic")
    .fallback(knobs::magic_level::no_magic)
    ;
    p.add(make_knob(settings))
    .desc("A settings file.")
    .name("settings")
    .fallback("-")
    ;
    p.add(make_knob(reference_files))
    .desc("A prefix for reference files.")
    .name("reference_file_prefix")
    .name("refs")
    ;
    return;
  }
};

int main(const int argc, const char** argv) {
  using namespace std;
  using namespace com::masaers::cmdlp;
  const char* preamble =
  "This can be a pretty long text about what the program does, that "
  "goes between the usage and the option descriptions. Just to give you "
  "an idea, I am going to ramble on for a while; although I really don't have "
  "that much to say.\nOh, and paragraphs should work to, so any newline will "
  "be interpreted as a paragraph break. Neat, huh? (Keep in mind that C++ "
  "transforms newlines when reading to and from fstreams, so it should be a "
  "control-n regardless of which platform you're on.)"
  ;
  const char* postamble =
  "Again, lay out the text if you will! Maybe add some credits "
  "(or copyright if you're not into the whole copyleft thing)."
  ;
  options<local_options> o(argc, argv, options_config().argdesc("file_0 (... file_n)").preamble(preamble).postamble(postamble));
  if (! o) {
    return o.exit_code();
  }

  for (string line; getline(*o.settings, line); /**/) {
    cout << "From settings: '" << line << "'" << endl;
  }

  {
    cout << "         1         2         3         4         5         6         7         8" << endl;;
    cout << "12345678901234567890123456789012345678901234567890123456789012345678901234567890" << endl;
    const auto p = com::masaers::cmdlp::paragraph(cout, 60, 2, 2);
    cout << "Lorem Ipsum är en utfyllnadstext från tryck- och förlagsindustrin. "
    "Lorem ipsum har varit standard ända sedan 1500-talet, "
    "när en okänd boksättare tog att antal bokstäver och blandade dem för "
    "att göra ett provexemplar av en bok. Lorem ipsum har inte bara överlevt "
    "fem århundraden, utan även övergången till elektronisk typografi utan "
    "större förändringar. Det blev allmänt känt på 1960-talet i samband med "
    "lanseringen av Letraset-ark med avsnitt av Lorem Ipsum, och senare med "
    "mjukvaror som Aldus PageMaker.\n"
    "The Skåne town of Råå has a stream (å) with eel (ål): ie Råååål. Hope "
    "that is enough unicode to give a bad linebreak."
    << endl;
  }
  switch (o.magic_level) {
    case knobs::magic_level::no_magic:
    break;
    case knobs::magic_level::less_magic:
    break;
    case knobs::magic_level::more_magic:
    break;
    default:
    ;
  }

  for (const auto& ref : o.reference_files) {
    cout << "Reference file: '" << ref << "'." << endl;
    size_t count = 0;
    for (string line; getline(*ref, line); ++count);
    cout << count << endl;
  }
  return EXIT_SUCCESS;
}


