/**
 * @file lexweight_test.cpp
 * @brief Unit test for lexweight.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "lexweight.h"
#include "util.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace yisi;

int main(int argc, char* argv[])
{

   if (argc == 1) {
      vector<vector<string> > tokens;
      tokens.push_back(tokenize("Wondering where you left your umbrella ?"));
      tokens.push_back(tokenize("Your laptop ?"));
      tokens.push_back(tokenize("Your wallet ?"));
      tokens.push_back(tokenize("Not to worry ."));
      tokens.push_back(tokenize("Toronto Pearson offers a secure Lost and Found service for all passengers and employees ."));

      lexweight_t idf(tokens);
      double w1 = idf("?");
      double w2 = idf("offers");
      cout << "The idf for '?' is " << w1 << endl;
      cout << "The idf for 'offers' is " << w2 << endl;

   } else {
      lexweight_t idf("learn", argv[1]);
      if (argc == 2) {
         idf.write(cout);
      } else {
         ofstream fout;
         fout.open(argv[2]);
         idf.write(fout);
      }
   }
   return 0;
}

