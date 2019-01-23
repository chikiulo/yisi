/**
 * @file util.h
 * @brief Declarations of some utility functions and global constants.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <functional>

namespace yisi {
   std::vector<std::string> tokenize(std::string sent, char d = ' ', bool keep_empty = false);
   std::string join(const std::vector<std::string> tokens, const std::string d = " ");
   std::vector<std::string> read_file(std::string filename);
   void open_ofstream(std::ofstream& fout, std::string filename);
   std::string lowercase(std::string token);
   std::pair<int, char**> str2charss(std::string str, char d = ' ');
   static inline std::string lstrip(std::string s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                            std::not1(std::ptr_fun<int, int>(std::isspace))));
      return s;
   }
   static inline std::string rstrip(std::string s) {
      s.erase(std::find_if(s.rbegin(), s.rend(),
                 std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
   }
   static inline std::string strip(std::string s) {
      auto ss = lstrip(rstrip(s));
      return ss;
   }
   static inline std::string debpe(std::string s) {
      auto ss = s;
      auto bpe = ss.find("@@ ");
      while (bpe != std::string::npos) {
         ss.erase(bpe, bpe + 3);
         bpe = ss.find("@@ ");
      }
      return ss;
   }
   static const int REF_MODE = 0;
   static const int HYP_MODE = 1;
   static const int INP_MODE = 2;
}

#endif
