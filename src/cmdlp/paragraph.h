/**
   This file is part of the command-line option library, which was cloned from:
   https://github.com/masaers/cmdlp (v0.4.1 tag)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */

#ifndef COM_MASAERS_CMDLP_PARAGRAPH_HPP
#define COM_MASAERS_CMDLP_PARAGRAPH_HPP
#include <iostream>
#include <sstream>
#include <string>

namespace com { namespace masaers { namespace cmdlp {

  /**
  This is pretty much where all the magic happens, but I don't consider
  it part of the API, so everything in it and its name is subject ot change
  without prior notice. Use the factory functions below instead.
  */
  template<typename Char> class pbuf;

  /**
  Creates a basic paragraph where lines are at most width wide.
  */
  template<typename Char> inline pbuf<Char> paragraph(
    /// The stream to modify.
    std::basic_ostream<Char>& stream,
    /// The column width. No line will be longer than this.
    const std::size_t width,
    /// The margin width. The number of spaces printed before the column
    /// content starts.
    const std::size_t margin);

  /**
  Creates a basic paragraph where paragraph breaks are represetned with an
  additional number of whitespaces.
  */
  template<typename Char> inline pbuf<Char> paragraph(
    /// The stream to modify.
    std::basic_ostream<Char>& stream,
    /// The column width. No line will be longer than this.
    const std::size_t width,
    /// The margin width. The number of spaces printed before the column
    /// content starts.
    const std::size_t margin,
    /// The number of spaces to indent (positive) or outdent (negative)
    /// the first line after a paragraph break.
    const int pbreak_margin_delta);

  /**
  Creates a hanging list, where the first line is not indented, but subsequent
  lines in the same paragraph are.
  */
  template<typename Char> inline pbuf<Char> hanging_list(
    /// The stream to modify.
    std::basic_ostream<Char>& stream,
    /// The column width. No line will be longer than this.
    const std::size_t width,
    /// The number of white spaces to indent subsequent lines.
    const std::size_t indent);


  template<typename Char>
  class pbuf : public std::basic_stringbuf<Char> {
  public:
    typedef std::basic_string<Char> string_type;
    typedef std::tuple<typename string_type::const_iterator, typename string_type::const_iterator, std::size_t> substring_type;
  protected:
    std::basic_streambuf<Char>* buf_m;
    std::basic_ostream<Char>* stream_m;
    std::size_t width_m;
    string_type pbreak_m;
    string_type margin_m;
    bool break_at_first_m;
    bool no_overflow_m;
    bool first_paragraph_m;
    virtual int sync() override {
      stream_m->rdbuf(buf_m);
      ragged_right(*stream_m, this->str());
      stream_m->flush();
      this->str("");
      stream_m->rdbuf(this);
      return 0;
    }
    static inline constexpr std::size_t substring_len(const substring_type& str) {
      return std::get<1>(str) - std::get<0>(str);
    }
    static inline void write_substring_to(std::basic_ostream<Char>& os, const substring_type& str) {
      os.write(&*std::get<0>(str), substring_len(str));
      // os.write(&*str.first, substring_len(str));
    }
    void ragged_right(std::basic_ostream<Char>& os, const string_type& str);
    void ragged_right_par(std::basic_ostream<Char>& os, const substring_type& par);
    template<typename Pred, typename InputIterator, typename OutputIterator>
    OutputIterator tokenize_to(const Pred& pred, const bool& skip_init, const InputIterator& first, const InputIterator& last, OutputIterator&& out);
  public:
    inline pbuf(std::basic_ostream<Char>& stream,
      const std::size_t& width,
      const string_type& pbreak,
      const string_type& margin,
      bool break_at_first,
      bool no_overflow)
    : buf_m(stream.rdbuf())
    , stream_m(&stream)
    , width_m(width)
    , pbreak_m(pbreak)
    , margin_m(margin)
    , break_at_first_m(break_at_first)
    , no_overflow_m(no_overflow)
    , first_paragraph_m(true)
    {
      stream_m->rdbuf(this);
    }
    inline pbuf(const pbuf&) = default;
    inline pbuf(pbuf&&) = default;
    inline pbuf& operator=(const pbuf&) = default;
    inline pbuf& operator=(pbuf&&) = default;
    inline ~pbuf() {
      stream_m->rdbuf(buf_m);
      *stream_m << this->str();
      stream_m->flush();
    }
  };

  template<typename Char>
  inline pbuf<Char> paragraph(std::basic_ostream<Char>& stream, const std::size_t width, const std::size_t margin) {
    return pbuf<Char>(stream,
                      width,
                      typename pbuf<Char>::string_type(margin, ' '),
                      typename pbuf<Char>::string_type(margin, ' '),
                      false,
                      false);
  }
  template<typename Char>
  inline pbuf<Char> paragraph(std::basic_ostream<Char>& stream, const std::size_t width, const std::size_t margin, const int pbreak_margin_delta) {
    if (margin + pbreak_margin_delta < 0 || width <= margin || width <= margin + pbreak_margin_delta) {
      throw std::runtime_error("Negative or zero space in paragraph!");
    }
    return pbuf<Char>(stream,
                      width,
                      typename pbuf<Char>::string_type(margin + pbreak_margin_delta, ' '),
                      typename pbuf<Char>::string_type(margin, ' '),
                      false,
                      false);
  }
  template<typename Char>
  inline pbuf<Char> hanging_list(std::basic_ostream<Char>& stream, const std::size_t width, const std::size_t indent) {
    return pbuf<Char>(stream,
                      width,
                      "",
                      typename pbuf<Char>::string_type(indent, ' '),
                      true,
                      false);
  }

} } }


template<typename Char>
void com::masaers::cmdlp::pbuf<Char>::ragged_right(std::basic_ostream<Char>& os,
                                                   const typename com::masaers::cmdlp::pbuf<Char>::string_type& str) {
  const auto& pred = [](const Char& c) { return c != std::char_traits<Char>::to_char_type('\n'); };
  std::vector<substring_type> pars;
  tokenize_to(pred, false, str.begin(), str.end(), std::back_inserter(pars));
  for (const auto& par : pars) {
    ragged_right_par(os, par);
    os << std::char_traits<Char>::to_char_type('\n');
    first_paragraph_m = false;
  }
}

template<typename Char>
void com::masaers::cmdlp::pbuf<Char>::ragged_right_par(std::basic_ostream<Char>& os,
                                                       const substring_type& par) {
  using namespace std;
  const auto& pred = [](const Char& c) { return ! isspace(c); };
  vector<substring_type> tokens;
  tokenize_to(pred, true, std::get<0>(par), std::get<1>(par), back_inserter(tokens));
  if (! tokens.empty()) {
    size_t len = 0;
    if (break_at_first_m || ! first_paragraph_m) {
      os << pbreak_m;
      len += pbreak_m.size();
    } else {
      os << margin_m;
      len += margin_m.size();
    }
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
      if (len + 1 + std::get<2>(*it) > width_m) {
        os << char_traits<Char>::to_char_type('\n') << margin_m;
        len = margin_m.size();
      }
      os << char_traits<Char>::to_char_type(' ');
      write_substring_to(os, *it);
      len += 1 + std::get<2>(*it);
    }
  }
}

template<typename Char>
template<typename Pred, typename InputIterator, typename OutputIterator>
OutputIterator com::masaers::cmdlp::pbuf<Char>::tokenize_to(const Pred& pred,
                                                            const bool& skip_init,
                                                            const InputIterator& first,
                                                            const InputIterator& last,
                                                            OutputIterator&& out) {
  InputIterator it = first;
  InputIterator token_begin;
  std::size_t token_len;
  if (skip_init) {
    for (/**/; it != last && ! pred(*it); ++it);
  } 
  token_begin = it;
  token_len = 0;
  while (it != last) {
    for (/**/; it != last && pred(*it); ++it) {
      ++token_len;
    }
    *out++ = substring_type(token_begin, it, token_len);
    for (/**/; it != last && ! pred(*it); ++it);
    token_begin = it;
    token_len = 0;
  }
  return std::forward<OutputIterator>(out);
}

template<>
template<typename Pred, typename InputIterator, typename OutputIterator>
OutputIterator com::masaers::cmdlp::pbuf<char>::tokenize_to(const Pred& pred,
                                                            const bool& skip_init,
                                                            const InputIterator& first,
                                                            const InputIterator& last,
                                                            OutputIterator&& out) {
  InputIterator it = first;
  InputIterator token_begin;
  std::size_t token_len;
  if (skip_init) {
    for (/**/; it != last && ! pred(*it); ++it);
  } 
  token_begin = it;
  token_len = 0;
  while (it != last) {
    for (/**/; it != last && pred(*it); ++it) {
      if ((*it & 0xc0) != 0x80) {
        ++token_len;
      }
    }
    *out++ = substring_type(token_begin, it, token_len);
    for (/**/; it != last && ! pred(*it); ++it);
    token_begin = it;
    token_len = 0;
  }
  return std::forward<OutputIterator>(out);
}


#endif
