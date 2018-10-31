/**
 * @file yisiscorer_test.cpp
 * @brief Unit test for yisiscorer.
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

#include "util.h"
#include "yisiscorer.h"
#include "srl.h"

using namespace std;
using namespace yisi;

int main(const int argc, const char* argv[])
{
   typedef com::masaers::cmdlp::options<yisi_options, phrasesim_options> options_type;

   options_type opt(argc,argv);
   if (! opt) {
      return opt.exit_code();
   }

   yisiscorer_t<options_type> yisi(opt);

   string reffile("test_ref.en");
   string hypfile("test_hyp.en");
   vector<string> refsents = read_file(reffile);
   vector<string> hypsents = read_file(hypfile);

   auto r1 = yisi.refsrlparse(refsents);
   auto r2 = yisi.hypsrlparse(hypsents);

   for (size_t i=0; i < r1.size(); i++) {
      //cerr << "Building YiSi graph" << endl;
      vector<srlgraph_t> rs;
      rs.push_back(r1[i]);
      yisigraph_t m = yisi.align(rs,r2[i]);

      cout << "YiSi score is:" << yisi.score(m) << endl;
   }
}
