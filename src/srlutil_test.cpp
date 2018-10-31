/**
 * @file srlutil_test.cpp
 * @brief Unit test for srlutil.
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
#include <fstream>

#include "srlutil.h"

using namespace std;
using namespace yisi;

int main(const int argc, const char* argv[])
{
   auto s = read_conll09batch(argv[1]);

   for (auto it=s.begin(); it!=s.end(); it++){
      cout << *it;
   }

   return 0;
}

