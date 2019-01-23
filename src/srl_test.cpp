/**
 * @file srl_test.cpp
 * @brief Unit test for srl.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "srl.h"

using namespace std;
using namespace yisi;

int main(const int argc, const char* argv[])
{
   if (argc == 1) {
      srl_t mate("mate", "parse_full_es.sh");

      vector<string> sents;

      ifstream IN("test_es.txt");
      if (IN.fail() or IN.bad()) {
         cerr << "ERROR: Failed to open: test_es.txt. Exiting..." << endl;
         exit(1);
      }

      while (!IN.eof()) {
         string line;
         getline(IN, line);
         if (line != "") {
            sents.push_back(line);
         }
      }

      auto r = mate.parse(sents);
      cout << "Done parsing " << r.size() << " srlgraphs." << endl;

      for (auto it = r.begin(); it != r.end(); it++) {
         cout << *it;
      }

   } else {
      srl_t parser(argv[1], argv[2]);
      vector<string> sents;

      ifstream IN(argv[3]);
      if (IN.fail() or IN.bad()) {
         cerr << "ERROR: Failed to open:" << argv[3] << ". Exiting..." << endl;
         exit(1);
      }

      while (!IN.eof()) {
         string line;
         getline(IN, line);
         if (line != "") {
            sents.push_back(line);
         }
      }
      IN.close();

      auto r = parser.parse(sents);
      cout << "Done parsing " << r.size() << " srlgraphs." << endl;

      ofstream OUT(argv[4]);
      for (int i = 0; i < (int)r.size(); i++) {
         r[i].print(OUT, i);
      }
      OUT.close();
   }

   return 0;
}

