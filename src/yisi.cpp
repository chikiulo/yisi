/**
 * @file yisi.cpp
 * @brief Main program for YiSi.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "cmdlp/options.h"
#include "yisiscorer.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;
using namespace yisi;

struct eval_options {
   std::string ref_file_m;
   std::string hyp_file_m;
   std::string inp_file_m;
   std::string sntscore_file_m;
   std::string docscore_file_m;
   std::string mode_m;

   void init(com::masaers::cmdlp::parser& p) {
      using namespace com::masaers::cmdlp;

      p.add(make_knob(ref_file_m))
         .fallback("")
         .desc("Filenames of references separated by ':'")
         .name("ref-file")
         ;
      p.add(make_knob(hyp_file_m))
         .desc("Filename of hypotheses")
         .name("hyp-file")
         ;
      p.add(make_knob(inp_file_m))
         .fallback("")
         .desc("Filename of input")
         .name("inp-file")
         ;
      p.add(make_knob(sntscore_file_m))
         .fallback("")
         .desc("Filename of sentence score output (default: <hyp-file>.scores)")
         .name("sntscore-file")
         ;
      p.add(make_knob(docscore_file_m))
         .fallback("")
         .desc("Filename of document score output (default: <sntscore-file>.doc")
         .name("docscore-file")
         ;
      p.add(make_knob(mode_m))
         .fallback("yisi")
         .desc("Output mode of YiSi [yisi(default): print score only "
               "| features: print feature weights and scores separated by white space]")
         .name("mode")
         ;
   }
}; // struct eval_options

int main(const int argc, const char* argv[])
{
   typedef com::masaers::cmdlp::options<eval_options, yisi_options, phrasesim_options> options_type;

   options_type opt(argc,argv);
   if (! opt) {
      return opt.exit_code();
   }

   if (opt.reflexweight_name_m == "learn" && opt.reflexweight_path_m == "") {
      opt.reflexweight_path_m = opt.ref_file_m;
   }
   if (opt.hyplexweight_name_m == "learn" && opt.hyplexweight_path_m == "") {
      opt.hyplexweight_path_m = opt.hyp_file_m;
   }
   if (opt.inplexweight_name_m == "learn" && opt.inplexweight_path_m == "") {
      opt.inplexweight_path_m = opt.inp_file_m;
   }

   yisiscorer_t<options_type> yisi(opt);

   if (opt.sntscore_file_m == "") {
      opt.sntscore_file_m = opt.hyp_file_m + ".sntyisi";
   }

   if (opt.docscore_file_m == "") {
      opt.docscore_file_m = opt.sntscore_file_m + ".docyisi";
   }

   ofstream SNTOUT;
   open_ofstream(SNTOUT, opt.sntscore_file_m);

   vector<string> hypsents = read_file(opt.hyp_file_m);

   vector<vector<string> > refsents;
   if (opt.ref_file_m != "") {
      auto reffiles = tokenize(opt.ref_file_m, ':');
      auto it = reffiles.begin();
      vector < string > rs = read_file(*it);
      if (rs.size() == hypsents.size()) {
         for (auto jt = rs.begin(); jt != rs.end(); jt++) {
            vector < string > ref;
            ref.push_back(*jt);
            refsents.push_back(ref);
         }
         it++;
         for (; it != reffiles.end(); it++) {
            rs = read_file(*it);
            if (rs.size() == hypsents.size()) {
               for (size_t j = 0; j < rs.size(); j++) {
                  refsents[j].push_back(rs[j]);
               }
            } else {
               cerr << "ERROR: No. of sentences in ref-file (" << rs.size()
                    << ") does not match with no. of sentences in hyp-file ("
                    << hypsents.size() << "). Check your input! Exiting ..."
                    << endl;
               exit(1);
            }
         }
      } else {
         cerr << "ERROR: No. of sentences in ref-file (" << rs.size()
              << ") does not match with no. of sentences in hyp-file ("
              << hypsents.size() << "). Check your input! Exiting ..." << endl;
         exit(1);
      }
   }

   vector<string> inpsents;
   if (opt.inp_file_m != "") {
      inpsents = read_file(opt.inp_file_m);
   }

   if (inpsents.size() > 0 && inpsents.size() != hypsents.size()) {
      cerr << "ERROR: No. of sentences in inp-file (" << inpsents.size()
              << ") does not match with no. of sentences in hyp-file ("
              << hypsents.size() << "). Check your input! Exiting..." << endl;
      exit(1);
   }

   cerr << "Tokenizing/SRL-ing hyp ... ";
   vector<srlgraph_t> hypsrlgraphs = yisi.hypsrlparse(hypsents);
   cerr << "Done." << endl;
   vector<vector<srlgraph_t> > refsrlgraphs;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      refsrlgraphs.push_back(vector<srlgraph_t>());
   }

   if (refsents.size() > 0) {
      cerr << "Tokenizing/SRL-ing ref ... ";
      for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
         refsrlgraphs[i] = yisi.refsrlparse(refsents[i]);
      }
      cerr << "Done." << endl;
   }

   vector<srlgraph_t> inpsrlgraphs;
   if (inpsents.size() > 0) {
      cerr << "Tokenizing/SRL-ing inp ... ";
      inpsrlgraphs = yisi.inpsrlparse(inpsents);
      cerr << "Done." << endl;
   }

   double docscore = 0.0;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      cout << "Evaluating line " << i + 1 << endl;
      yisigraph_t m;
      if (opt.inp_file_m != "") {
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i], inpsrlgraphs[i]);
      } else {
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i]);
      }
      if (opt.mode_m != "features") {
         double s = yisi.score(m);
         SNTOUT << s << endl;
         docscore += s;
      } else {
         auto f = yisi.features(m);
         for (auto it = f.begin(); it != f.end(); it++) {
            SNTOUT << *it << " ";
         }
         SNTOUT << endl;
      }
   }
   SNTOUT.close();

   if (opt.mode_m != "features") {
      ofstream DOCOUT;
      open_ofstream(DOCOUT, opt.docscore_file_m);
      docscore /= hypsents.size();
      DOCOUT << docscore << endl;
      DOCOUT.close();
   }

   return 0;
}
