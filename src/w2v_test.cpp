/**
 * @file w2v_test.cpp
 * @brief Unit test for w2v lexsim.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "lexsim.h"

#include <iostream>

using namespace std;
using namespace yisi;

int main(int argc, char* argv[])
{
   lexsim_t w2vtxt("w2v", argv[1], "cosine");
   if (argc == 3) {
      w2vtxt.write_txtw2v(argv[2]);
   }

   string s1;
   string s2;

   if (argc > 3) {
      s1 = argv[2];
      s2 = argv[3];
      cout << "Sim = " << w2vtxt.get_sim(s1, s2, yisi::REF_MODE) << endl;

   } else {
      while (true) {
         cout << "Input two strings:" << endl;
         cin >> s1;
         cin >> s2;
         cout << "Sim = " << w2vtxt.get_sim(s1, s2, yisi::REF_MODE) << endl;
      }
   }

   return 0;
}

