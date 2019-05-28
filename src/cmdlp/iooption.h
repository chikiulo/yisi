/**
   This file is part of the command-line option library, which was cloned from:
   https://github.com/masaers/cmdlp (v0.4.1 tag)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */

#ifndef COM_MASAERS_CMDLP_IOOPTION_HPP
#define COM_MASAERS_CMDLP_IOOPTION_HPP
#include "cmdlp.h"

#include <iostream>
#include <sstream>
#include <string>
#include <memory>

namespace com { namespace masaers { namespace cmdlp {
  const char stdio_filename[] = "-";
  template<typename Char, typename Traits = std::char_traits<Char> > class basic_ifile;
  template<bool IsOptional, typename Char, typename Traits = std::char_traits<Char> > class basic_ofile;
  template<typename Char, typename Traits = std::char_traits<Char> > class basic_ifile_prefix;

  using ifile          = basic_ifile<char>;
  using ofile          = basic_ofile<false, char>;
  using optional_ofile = basic_ofile<true, char>;
  using ifile_prefix   = basic_ifile_prefix<char>;


  template<typename Char, typename Traits>
  class basic_ifile {
  public:
    inline basic_ifile() : filename_m(), ifsp_m(nullptr), sp_m(nullptr) {}
    inline basic_ifile(const std::string& filename) : filename_m(filename), ifsp_m(nullptr), sp_m(nullptr) {}
    inline basic_ifile(const basic_ifile&) = default;
    inline basic_ifile(basic_ifile&&) = default;
    inline basic_ifile& operator=(const basic_ifile&) = default;
    inline basic_ifile& operator=(basic_ifile&&) = default;
    inline std::basic_istream<Char, Traits>& operator*() const { return *sp_m; }
    inline std::basic_istream<Char, Traits>* operator->() const { return sp_m; }
    template<typename C, typename T>
    friend inline std::basic_istream<C, T>&
    operator>>(std::basic_istream<C, T>& is, basic_ifile& x) {
      return is >> x.filename_m;
    }
    template<typename C, typename T>
    friend inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const basic_ifile& x) {
      return os << x.filename_m;
    }
    inline bool validate() {
      bool result = false;
      if (! filename_m.empty()) {
        if (filename_m == stdio_filename) {
          sp_m = &std::cin;
          result = true;
        } else {
          ifsp_m.reset(new std::basic_ifstream<Char, Traits>(filename_m));
          sp_m = ifsp_m.get();
          result = true;
        }
      }
      return result;
    }
  protected:
    std::string filename_m;
    std::shared_ptr<std::basic_ifstream<Char, Traits> > ifsp_m;
    std::basic_istream<Char, Traits>* sp_m;
  }; // basic_ifile

  template<bool IsOptional, typename Char, typename Traits>
  class basic_ofile {
  public:
    inline basic_ofile() : filename_m(), ofsp_m(nullptr), sp_m(nullptr) {}
    inline basic_ofile(const std::string& filename) : filename_m(filename), ofsp_m(nullptr), sp_m(nullptr) {}
    inline basic_ofile(const basic_ofile&) = default;
    inline basic_ofile(basic_ofile&&) = default;
    inline basic_ofile& operator=(const basic_ofile&) = default;
    inline basic_ofile& operator=(basic_ofile&) = default;
    inline operator bool() const { return sp_m != nullptr; }
    inline std::basic_ostream<Char, Traits>& operator*() const { return *sp_m; }
    inline std::basic_ostream<Char, Traits>* operator->() const { return sp_m; }
    template<typename C, typename T>
    friend inline std::basic_istream<C, T>&
    operator>>(std::basic_istream<C, T>& is, basic_ofile<IsOptional, Char, Traits>& x) {
      return is >> x.filename_m;
    }
    template<typename C, typename T>
    friend inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, const basic_ofile<IsOptional, Char, Traits>& x) {
      return os << x.filename_m;
    }
    inline bool validate() {
      bool result = IsOptional;
      if (! filename_m.empty()) {
        if (filename_m == stdio_filename) {
          sp_m = &std::cout;
          result = true;
        } else {
          ofsp_m.reset(new std::basic_ofstream<Char, Traits>(filename_m));
          sp_m = ofsp_m.get();
          result = true;
        }
      }
      return result;
    }
  protected:
    std::string filename_m;
    std::shared_ptr<std::basic_ofstream<Char, Traits> > ofsp_m;
    std::basic_ostream<Char, Traits>* sp_m;
  }; // basic_ofile


  bool expand_prefix(const std::string& prefix_path, std::vector<std::string>& filennames);

  template<typename Char, typename Traits>
  class basic_ifile_prefix {
  public:
    typedef basic_ifile<Char, Traits> ifile_type;
    typedef std::vector<ifile_type> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    inline basic_ifile_prefix() : files_m() {}
    inline basic_ifile_prefix(const std::string& prefix) : prefix_m(prefix) {}
    inline basic_ifile_prefix(const basic_ifile_prefix&) = default;
    inline basic_ifile_prefix(basic_ifile_prefix&&) = default;
    inline basic_ifile_prefix& operator=(const basic_ifile_prefix&) = default;
    inline basic_ifile_prefix& operator=(basic_ifile_prefix&&) = default;
    template<typename C, typename T>
    friend inline std::basic_istream<C, T>&
    operator>>(std::basic_istream<C, T>& is, basic_ifile_prefix& x) {
      return is >> x.prefix_m;
    }
    template<typename C, typename T>
    friend inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& os, basic_ifile_prefix& x) {
      return os << x.prefix_m;
    }
    inline const_iterator cbegin() const { return files_m.begin(); }
    inline const_iterator begin() const { return files_m.begin(); }
    inline iterator begin() { return files_m.begin(); }
    inline const_iterator cend() const { return files_m.end(); }
    inline const_iterator end() const { return files_m.end(); }
    inline iterator end() { return files_m.end(); }
    inline bool validate() {
      std::vector<std::string> filenames;
      bool result = expand_prefix(prefix_m, filenames);
      for (const auto& filename : filenames) {
        files_m.push_back(filename);
        result = result && files_m.back().validate();
      }
      return result;
    }
  protected:
    std::string prefix_m;
    container_type files_m;
  }; // ifile_prefix


  template<typename Char, typename Traits>
  inline value_option<basic_ifile<Char, Traits> >
  make_knob(basic_ifile<Char, Traits>& value) {
    return value_option<basic_ifile<Char, Traits> >(value)
    .validator([](basic_ifile<Char, Traits>& x) { return x.validate(); })
    ;
  }

  template<bool IsOptional, typename Char, typename Traits>
  inline value_option<basic_ofile<IsOptional, Char, Traits> >
  make_knob(basic_ofile<IsOptional, Char, Traits>& value) {
    return value_option<basic_ofile<IsOptional, Char, Traits> >(value)
    .validator([](basic_ofile<IsOptional, Char, Traits>& x) { return x.validate(); })
    ;
  }

  template<typename Char, typename Traits>
  inline value_option<basic_ifile_prefix<Char, Traits> >
  make_knob(basic_ifile_prefix<Char, Traits>& value) {
    return value_option<basic_ifile_prefix<Char, Traits> >(value)
    .validator([](basic_ifile_prefix<Char, Traits>& value){ return value.validate(); })
    ;
  }


} } }

#endif
