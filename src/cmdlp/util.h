/**
   This file is part of the command-line option library, which was cloned from:

   https://github.com/masaers/cmdlp (v0.2 tag)

   Thanks Markus!
   Consider cloning the original repository if you like it.

   Copyright (c) 2018 Markus S. Saers
 */

#ifndef UTIL_HPP
#define UTIL_HPP
#include <iterator>

struct null_output_iterator
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
  template<typename T>
  inline const null_output_iterator& operator=(T&&) const { return *this; }
  inline const null_output_iterator& operator*() const { return *this; }
  inline const null_output_iterator& operator++() const { return *this; }
  inline const null_output_iterator& operator++(int) const { return *this; }
};

template<typename T>
class assign_iterator
  : public std::iterator<std::output_iterator_tag, T>
{
public:
  inline assign_iterator() : x_m(NULL) {}
  explicit inline assign_iterator(T& x) : x_m(&x) {}
  inline assign_iterator(const assign_iterator&) = default;
  inline T& operator*() const { return *x_m; }
  inline const assign_iterator& operator++() const { return *this; }
  inline const assign_iterator& operator++(int) const { return *this; }
private:
  T* x_m;
};

#define VARLOG(...) std::cerr << __FILE__ << ':' << __LINE__ << " " << #__VA_ARGS__  << " [" << (__VA_ARGS__) << "]" << std::endl

#endif
