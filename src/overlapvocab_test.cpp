/**
 * @file lexsim_test.cpp
 * @brief Unit test for lexsim.
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
   string inpembpath = argv[1];
   string hypembpath = argv[2];
   map<string, vector<double> > inpemb;
   map<string, vector<double> > hypemb;
   int dim;
   read_binw2v(inpembpath, inpemb, dim);
   read_binw2v(hypembpath, hypemb, dim);

   auto it = inpemb.begin();
   auto jt = hypemb.begin();
   while (it != inpemb.end() && jt != hypemb.end()) {
      string inp = it->first;
      string hyp = jt->first;
      if (inp == hyp) {
         cout << inp << " " << hyp << endl;
         it++;
         jt++;
      } else if (inp.compare(hyp) < 0) {
         it++;
      } else {
         jt++;
      }
   }

   return 0;
}

