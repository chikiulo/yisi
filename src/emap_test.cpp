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
#include <set>

using namespace std;
using namespace yisi;

int main(int argc, char* argv[])
{
   string inpembpath = argv[1];
   string hypembpath = argv[2];
   string inpmappath = argv[3];
   string inpdocpath = argv[4];
   ofstream INPMAP;
   ofstream INPDOC;
   open_ofstream(INPMAP, inpmappath);

   map<string, vector<double> > inpemb;
   map<string, vector<double> > hypemb;
   map<string, vector<double> > inpfilemb;
   int dim;
   read_binw2v(inpembpath, inpemb, dim);
   read_binw2v(hypembpath, hypemb, dim);
   vector<string> inpsents = read_file(inpdocpath);
   //filter the inp emb according to the inp doc
   set<string> tokens;
   for (auto it = inpsents.begin(); it != inpsents.end(); it++) {
      auto sent = tokenize(*it);
      tokens.insert(sent.begin(), sent.end());
   }
   for (auto it = tokens.begin(); it != tokens.end(); it++) {
      auto jt = inpemb.find(*it);
      if (jt != inpemb.end()) {
         inpfilemb[*it] = jt->second;
      }
   }

   string maxsim_str;
   double maxsim_scr=0.0;
   for (auto it = inpfilemb.begin(); it != inpfilemb.end(); it++) {
      auto inp_s = it->first;
      auto inp_v = it->second;
      for (auto jt = hypemb.begin(); jt != hypemb.end(); jt++) {
         auto hyp_s = jt->first;
         auto hyp_v = jt->second;
         double sim = 0.0;
         for (int i = 0; i < dim; i++) {
            sim += inp_v[i] * hyp_v[i];
         }
         if (sim > maxsim_scr) {
            maxsim_str = hyp_s;
            maxsim_scr = sim;
         }
      }
      INPMAP << inp_s << "\t" << maxsim_str << endl;
      maxsim_scr = 0.0;
   }
   return 0;
}

