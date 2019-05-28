/**
 * @file util.cpp
 * @brief Utility functions.
 *
 * @author Jackie Lo
 *
 * The definitions of some utility functions.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "util.h"

#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

using namespace yisi;
using namespace std;

vector<string> yisi::tokenize(string sent, char d, bool keep_empty) {
   //cerr << "Tokenizing " << sent << " by " << d << endl;
   vector<string> result;
   istringstream iss(sent);
   while (!iss.eof()) {
      string token;
      getline(iss, token, d);
      if (token != "" || keep_empty) {
         result.push_back(token);
      }
   }
   //cerr << endl;
   return result;
}

string yisi::join(const vector<string> tokens, const string d) {
   string result = "";
   for (auto it = tokens.begin(); it != tokens.end(); ++it) {
      if (it != tokens.begin())
         result += d;
      result += *it;
   }
   return result;
}

vector<vector<string> > yisi::collect_ngram(int n, vector<string>& tokens) {
  vector <vector<string> > result;
  for (int i = 0; i <= (int)tokens.size() - n; i++) {
    vector <string > ngram;
    for (int j = i; j < i + n; j++) {
      ngram.push_back(tokens[j]);
    }
    result.push_back(ngram);
  }
  return result;
}

vector<string> yisi::read_file(string filename) {
   vector<string> result;
   ifstream fin(filename.c_str());
   if (!fin) {
      cerr << "ERROR: Failed to open input file (" << filename << "). Exiting..." << endl;
      exit(1);
   }
   while (!fin.eof()) {
      string line;
      getline(fin, line);
      result.push_back(line);
   }
   fin.close();
   result.pop_back();
   return result;
}

void yisi::open_ofstream(ofstream& fout, string filename) {
   fout.open(filename.c_str());
   if (!fout) {
      cerr << "ERROR: Failed to open output file (" << filename << "). Exiting..." << endl;
      exit(1);
   }
}

string yisi::lowercase(string token) {
   string result;
   for (auto it = token.begin(); it != token.end(); it++) {
      char c = *it;
      char r = tolower(c);
      result.push_back(r);
   }
   return result;
}


pair<int, char**> yisi::str2charss(string str, char d) {
   auto str_vec = yisi::tokenize(str, d);
   size_t n = str_vec.size();
   char** result = new char*[n];

   for (size_t i = 0; i < n; i++) {
      size_t len = strlen(str_vec[i].c_str()) + 10;
      result[i] = new char[len];
      cerr << str_vec[i].c_str() << endl;
      strcpy(result[i], str_vec[i].c_str());
   }

   return make_pair((int)n, result);
}

