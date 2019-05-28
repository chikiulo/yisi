/**
   This file is part of the command-line option library, which was cloned from:
   https://github.com/masaers/cmdlp (v0.4.1 tag)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */

#ifndef COM_MASAERS_CMDLP_OPTIONS_HPP
#define COM_MASAERS_CMDLP_OPTIONS_HPP
#include "cmdlp.h"
#include "iooption.h"
#include "paragraph.h"
// c++
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
// c
#include <cstdlib>

namespace com { namespace masaers { namespace cmdlp {
  namespace options_helper {
    template<typename me_T, typename parser_T>
    inline void init_bases(me_T&, parser_T&) {}
    template<typename me_T, typename parser_T, typename T, typename... Ts>
    inline void init_bases(me_T& me, parser_T& p) {
      static_cast<T&>(me).init(p);
      init_bases<me_T, parser_T, Ts...>(me, p);
    }
  } // namespace options_helper
  
  class options_config {
  public:
    options_config() : dumpto_m(true), config_m(true), help_m(true), argdesc_m(), preamble_m(), postamble_m() {}
    options_config& no_dumpto() { dumpto_m = false; return *this; }
    options_config& no_config() { config_m = false; return *this; }
    options_config& no_help()   { help_m   = false; return *this; }
    options_config& argdesc  (const std::string& desc) {   argdesc_m = desc; return *this; }
    options_config& preamble (const std::string& text) {  preamble_m = text; return *this; }
    options_config& postamble(const std::string& text) { postamble_m = text; return *this; }
    const bool& dumpto() const { return dumpto_m; }
    const bool& config() const { return config_m; }
    const bool& help()   const { return help_m;   }
    const std::string& argdesc() const { return argdesc_m; }
    const std::string& preamble() const { return preamble_m; }
    const std::string& postamble() const { return postamble_m; }
  private:
    bool dumpto_m;
    bool config_m;
    bool help_m;
    std::string argdesc_m;
    std::string preamble_m;
    std::string postamble_m;
  };

  template<typename... options_T>
  class options : public options_T... {
  public:
    inline options(const int argc, const char** argv, const options_config& cfg);
    inline options(const int argc, char** argv, const options_config& cfg)
    : options(argc, (const char**)argv, cfg) {}
    inline options(const int argc, const char** argv)
    : options(argc, argv, options_config()) {}
    inline options(const int argc, char** argv)
    : options(argc, (const char**)argv, options_config()) {}
    inline operator bool() const { return ! help_needed(); }
    inline int exit_code() const { return error_count_m == 0 ? EXIT_SUCCESS : EXIT_FAILURE; }
    std::vector<std::string> args;
  private:
    inline bool help_needed() const { return help_requested_m || error_count_m != 0; }
    bool help_requested_m;
    std::size_t error_count_m;
  }; // options
  
} } }

template<typename... options_T> 
com::masaers::cmdlp::options<options_T...>::options(const int argc, const char** argv, const options_config& cfg)
: options_T()..., help_requested_m(false), error_count_m(0) {
  using namespace std;
  optional_ofile dumpto;
  config_files configs;
  parser p(cerr);
  options_helper::init_bases<options<options_T...>, parser, options_T...>(*this, p);
  if (cfg.dumpto()) {
    p.add(make_knob(dumpto))
    .name("dumpto")
    .desc("Dumps the parameters, as undestood by the program, to a config file "
      "that can later be used to rerun with the same settings. Leave empty to not dump. "
      "Use '-' to dump to standard output.")
    .fallback()
    .is_meta()
    ;
  }
  if (cfg.config()) {
    p.add(make_knob(configs))
    .name("config")
    .desc("Read parameters from the provided file as if they were provided in the same position on the command line.")
    ;
  }
  if (cfg.help()) {
    p.add(make_onswitch(help_requested_m))
    .name('h', "help")
    .desc("Prints the help message and exits normally.")
    .is_meta()
    ;
  }
  error_count_m += p.parse(argc, argv, back_inserter(args));
  error_count_m += p.validate();
  if (help_needed()) {
    cerr << endl << "usage: " << argv[0] << p.usage();
    if (! cfg.argdesc().empty()) {
      cerr << ' ' << cfg.argdesc();
    }
    cerr << endl << endl;
    if (! cfg.preamble().empty()) {
      const auto p = paragraph(cerr, 72, 4, 2);
      cerr << cfg.preamble() << endl;
    }
    cerr << p.help() << endl;
    if (! cfg.postamble().empty()) {
      const auto p = paragraph(cerr, 72, 4, 2);
      cerr << cfg.postamble() << endl;
    }
  }
  if (dumpto) {
    p.dumpto_stream(*dumpto, false);
  }
}

#endif
