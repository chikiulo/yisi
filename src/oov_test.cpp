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

#include "lexsim.h"

#include <iostream>

using namespace std;
using namespace yisi;

int main(int argc, char* argv[])
{
   lexsim_t w2vtxt("w2v", argv[1], "cosine");
   string sent;

   while(!cin.eof()){
      getline(cin, sent);
      //cout << sent << endl;
      auto tokens = tokenize(sent);
      for (auto it = tokens.begin(); it != tokens.end(); it++){
         if ((w2vtxt.get_wv(*it,HYP_MODE)).size() == 0){
            cout << *it << endl;
         }
      }
   }

   return 0;
}

