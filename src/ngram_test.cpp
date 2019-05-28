/**
 * @file w2v_test.cpp
 * @brief Unit test for w2v lexsim.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#include "util.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>

using namespace std;
using namespace yisi;

int main(int argc, char* argv[])
{
   set<string> result;
   while (!cin.eof()) {
      string line;
      cin >> line;
      auto tokens = tokenize(line);
      auto ngrams = collect_ngram(atoi(argv[1]), tokens);
      for (auto it = ngrams.begin(); it != ngrams.end(); it++) {
         auto ngram = join(*it);
         result.insert(ngram);
      }
   }
   for (auto it = result.begin(); it != result.end(); it++) {
      cout << *it << endl;
   }

   return 0;
}

