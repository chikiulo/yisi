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
   std::string ref_type_m;
   std::string hyp_type_m;
   std::string inp_type_m;

   std::string ref_file_m;
   std::string hyp_file_m;
   std::string inp_file_m;
   std::string inpunit_file_m;
   std::string refunit_file_m;
   std::string hypunit_file_m;
   std::string inpidemb_file_m;
   std::string refidemb_file_m;
   std::string hypidemb_file_m;

   std::string sntscore_file_m;
   std::string docscore_file_m;

   std::string mode_m;

   void init(com::masaers::cmdlp::parser& p) {
      using namespace com::masaers::cmdlp;
      p.add(make_knob(ref_type_m))
         .fallback("word")
         .desc("Type of reference sentences. [word(default)|unit|uemb]")
         .name("ref-type")
         ;
      p.add(make_knob(hyp_type_m))
         .fallback("word")
         .desc("Type of hypothese sentences. [word(default)|unit|uemb]")
         .name("hyp-type")
         ;
      p.add(make_knob(inp_type_m))
         .fallback("word")
         .desc("Filename of input. [word(default)|unit|uemb]")
         .name("inp-type")
         ;
      p.add(make_knob(ref_file_m))
         .fallback("")
         .desc("Filenames of references separated by ':'. (in surface word form for SRL.)")
         .name("ref-file")
         ;
      p.add(make_knob(hyp_file_m))
         .desc("Filename of hypotheses. (in surface word form for SRL.)")
         .name("hyp-file")
         ;
      p.add(make_knob(inp_file_m))
         .fallback("")
         .desc("Filename of input. (in surface word form for SRL.)")
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
      p.add(make_knob(inpunit_file_m))
         .fallback("")
         .desc("Filename to input segmented in subword units.")
         .name("inpunit-file")
         ;
      p.add(make_knob(hypunit_file_m))
         .fallback("")
         .desc("Filename to hypotheses segmented in subword units.")
         .name("hypunit-file")
         ;
      p.add(make_knob(refunit_file_m))
         .fallback("")
         .desc("Filename to reference segmented in subword units separated by ':'.")
         .name("refunit-file")
         ;
      p.add(make_knob(inpidemb_file_m))
         .fallback("")
         .desc("Filename to input subword units with contextual embeddings: one unit per line, "
               "empty line separates sentences [unitid<TAB>tokenid<TAB>space_sep_emb].")
         .name("inpidemb-file")
         ;
      p.add(make_knob(hypidemb_file_m))
         .fallback("")
         .desc("Filename to hypotheses subword units with contextual embeddings: one unit per line, "
               "empty line separates sentences [unitid<TAB>tokenid<TAB>space_sep_emb].")
         .name("hypidemb-file")
         ;
      p.add(make_knob(refidemb_file_m))
         .fallback("")
         .desc("Filename to reference subword units with contextual embeddings separated by ':': one "
               "unit per line, empty line separates sentences [unitid<TAB>tokenid<TAB>space_sep_emb].")
         .name("refidemb-file")
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

   options_type opt(argc, argv);
   if (!opt) {
      return opt.exit_code();
   }

   if (opt.reflexweight_name_m == "learn" && opt.reflexweight_path_m == "") {
      if (opt.ref_type_m == "word") {
         opt.reflexweight_path_m = opt.ref_file_m;
      } else {
         opt.reflexweight_path_m = opt.refunit_file_m;
      }
   }
   if (opt.hyplexweight_name_m == "learn" && opt.hyplexweight_path_m == "") {
      if (opt.hyp_type_m == "word") {
         opt.hyplexweight_path_m = opt.hyp_file_m;
      } else {
         opt.hyplexweight_path_m = opt.hypunit_file_m;
      }
   }
   if (opt.inplexweight_name_m == "learn" && opt.inplexweight_path_m == "") {
      if (opt.inp_type_m == "word") {
         opt.inplexweight_path_m = opt.inp_file_m;
      } else {
         opt.inplexweight_path_m = opt.inpunit_file_m;
      }
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

   cerr << "Reading hyp sents... ";
   vector<sent_t*> hypsents = read_sent(opt.hyp_type_m, opt.hyp_file_m, opt.hypunit_file_m, opt.hypidemb_file_m);
   cerr << "Done." << endl;

   vector < vector<sent_t*> > refsents;
   if (opt.ref_file_m != "") {
      cerr << "Reading ref sents... ";
      auto reffiles = tokenize(opt.ref_file_m, ':');
      auto refunits = tokenize(opt.refunit_file_m, ':');
      auto refidemb = tokenize(opt.refidemb_file_m, ':');
      size_t i = 0;
      vector<sent_t*> rs;
      if (reffiles.size() == refunits.size()) {
         rs = read_sent(opt.ref_type_m, reffiles[i], refunits[i], refidemb[i]);
      } else {
         rs = read_sent(opt.ref_type_m, reffiles[i]);
      }
      if (rs.size() == hypsents.size()) {
         for (auto jt = rs.begin(); jt != rs.end(); jt++) {
            vector<sent_t*> ref;
            ref.push_back(*jt);
            refsents.push_back(ref);
         }
         i++;
         for (; i < reffiles.size(); i++) {
            rs = read_sent(opt.ref_type_m, reffiles[i], refunits[i], refidemb[i]);
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
      cerr << "Done." << endl;
   }

   vector<sent_t*> inpsents;
   if (opt.inp_file_m != "") {
      cerr << "Reading inp sents... ";
      inpsents = read_sent(opt.inp_type_m, opt.inp_file_m, opt.inpunit_file_m, opt.inpidemb_file_m);
      if (inpsents.size() != hypsents.size()) {
         cerr << "ERROR: No. of sentences in inp-file (" << inpsents.size()
            << ") does not match with no. of sentences in hyp-file ("
            << hypsents.size() << "). Check your input! Exiting..." << endl;
         exit(1);
      }
      cerr << "Done." << endl;
   }

   cerr << "Creating hyp srlgraphs... ";
   vector<srlgraph_t> hypsrlgraphs = yisi.hypsrlparse(hypsents);
   cerr << "Done." << endl;
   vector < vector<srlgraph_t> > refsrlgraphs;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      refsrlgraphs.push_back(vector<srlgraph_t>());
   }

   if (refsents.size() > 0) {
      cerr << "Creating ref srlgraphs... ";
      for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
         refsrlgraphs[i] = yisi.refsrlparse(refsents[i]);
      }
      cerr << "Done." << endl;
   }

   vector<srlgraph_t> inpsrlgraphs;
   if (inpsents.size() > 0) {
      cerr << "Creating inp srlgraphs... ";
      inpsrlgraphs = yisi.inpsrlparse(inpsents);
      cerr << "Done." << endl;
   }

   double docscore = 0.0;

   for (size_t i = 0; i < hypsrlgraphs.size(); i++) {
      cout << "Evaluating line " << i + 1 << endl;
      yisigraph_t m;
      if (opt.inp_file_m != "") {
         /*
          cerr<<"inpsrlgraph:"<<endl;
          inpsrlgraphs[i].print(cout, i);
          cerr<<"hypsrlgraph:"<<endl;
          hypsrlgraphs[i].print(cout, i);
          cerr<<"yisigraph:"<<endl;
          */
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i], inpsrlgraphs[i]);
         // m.print(cout);
      } else {
         // hypsrlgraphs[i].print(cout, i);
         m = yisi.align(refsrlgraphs[i], hypsrlgraphs[i]);
         // m.print(cout);
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

   for (auto it = hypsents.begin(); it != hypsents.end(); it++) {
      delete *it;
      *it = NULL;
   }
   for (auto it = refsents.begin(); it != refsents.end(); it++) {
      for (auto jt = it->begin(); jt != it->end(); jt++) {
         delete *jt;
         *jt = NULL;
      }
   }
   for (auto it = inpsents.begin(); it != inpsents.end(); it++) {
      delete *it;
      *it = NULL;
   }

   return 0;
}
