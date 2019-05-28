/**
 * @file srlgraph_test.cpp
 * @brief Unit test for srlgraph.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "srlutil.h"

#include <iostream>
#include <fstream>
#include <string>

#include <vector>

using namespace std;
using namespace yisi;

int main(int argc, char* argv[]) {

   vector<sent_t*> sents = read_sent("word", string(argv[1]));

   cout << "Reading ASSERT format parse file." << endl;
   vector<srlgraph_t> srls = read_srl(sents, string(argv[2]));

   cout << "Printing srl parses:" << endl;
   for (auto it = srls.begin(); it != srls.end(); it++) {
      cout << (*it);
   }
   for (auto it = sents.begin(); it != sents.end(); it++) {
      delete *it;
      *it = NULL;
   }

   return 0;
}
