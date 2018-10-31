/**
 * @file phrasesim_test.cpp
 * @brief Unit test for phrasesim.
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
#include <string>

#include "phrasesim.h"

using namespace std;
using namespace yisi;

int main(const int argc, const char* argv[])
{
   typedef com::masaers::cmdlp::options<phrasesim_options> options_type;

   options_type opt(argc,argv);
   if (! opt) {
      return opt.exit_code();
   }

   phrasesim_t<options_type> phrasesim(opt);

   vector<string> s1{"EU"};
   vector<string> s2{"UK"};
   vector<string> s3{"EU", "government"};
   vector<string> s4{"UK", "government"};

   auto p1 = phrasesim(s1, s2, yisi::REF_MODE);
   auto p2 = phrasesim(s3, s4, yisi::REF_MODE);

   cout << "The similarity of 'EU' and 'UK' is: ("
        << p1.first << "," << p1.second << ")" << endl;
   cout <<  "The similarity of 'EU government' and 'UK government' is: ("
        << p2.first << "," << p2.second << ")" << endl;
}

