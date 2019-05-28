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

      vector<sent_t*> sents = read_sent("word", "test_es.txt");

      auto r = mate.parse(sents);
      cout << "Done parsing " << r.size() << " srlgraphs." << endl;

      for (auto it = r.begin(); it != r.end(); it++) {
         cout << *it;
      }

   } else {
      srl_t parser(argv[1], argv[2]);
      vector<sent_t*> sents = read_sent("word", string(argv[3]));

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

