/**
 * @file phrasesim.h
 * @brief Phrasal similarity
 *
 * @author Jackie Lo
 *
 * Class definition of phrasal similarity classes:
 *    - phrasesim_options (command line options for phrasesim)
 *    - phrasesim_t 
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef PHRASESIM_H
#define PHRASESIM_H

#include "cmdlp/cmdlp.h"
#include "lexsim.h"
#include "lexweight.h"
#include "maxmatching.h"

#include <string>
#include <vector> 
#include <iostream>
#include <fstream>
#include <cmath>
#include <utility>
#include <algorithm>

namespace yisi {

   struct phrasesim_options {
      std::string lexsim_name_m;
      std::string outlexsim_path_m;
      std::string inplexsim_path_m;
      std::string inplexweight_name_m;
      std::string inplexweight_path_m;
      std::string reflexweight_name_m;
      std::string reflexweight_path_m;
      std::string hyplexweight_name_m;
      std::string hyplexweight_path_m;
      std::string phrasesim_name_m;

      int n_m;

      void init(com::masaers::cmdlp::parser& p) {
         using namespace com::masaers::cmdlp;

         p.add(make_knob(lexsim_name_m))
            .fallback("exact")
            .desc("Type of lex sim model: [exact(default)|ibm1|w2v|ibmw2v]")
            .name("lexsim-type")
            ;
         p.add(make_knob(outlexsim_path_m))
            .fallback("")
            .desc("Path to lex sim model file in output language")
            .name("outlexsim-path")
            ;
         p.add(make_knob(inplexsim_path_m))
            .fallback("")
            .desc("Path to lex sim model file in input language")
            .name("inplexsim-path")
            ;
         p.add(make_knob(inplexweight_name_m))
            .fallback("uniform")
            .desc("Type of input lex weight model: [uniform(default)|file|learn]")
            .name("inplexweight-type")
            ;
         p.add(make_knob(inplexweight_path_m))
            .fallback("")
            .desc("[file: path to input lex weight model file "
                  "| learn: monolingual corpus in input language to learn]")
            .name("inplexweight-path")
            ;
         p.add(make_knob(reflexweight_name_m))
            .fallback("uniform")
            .desc("Type of reference lex weight model: [uniform(default)|file|learn]")
            .name("lexweight-type")
            .name("reflexweight-type")
            ;
         p.add(make_knob(reflexweight_path_m))
            .fallback("")
            .desc("[file: path to reference lex weight model file "
                  "| learn: monolingual corpus in reference language to learn]")
            .name("lexweight-path")
            .name("reflexweight-path")
            ;
         p.add(make_knob(hyplexweight_name_m))
            .fallback("")
            .desc("Type of hypotheses lex weight model: [uniform|file|learn] "
                  "(default: same as reflexweight-type")
            .name("hyplexweight-type")
            ;
         p.add(make_knob(hyplexweight_path_m))
            .fallback("")
            .desc("[file: path to hypotheses lex weight model file "
                  "| learn: monolingual corpus in hypothesis language to learn]")
            .name("hyplexweight-path")
            ;
         p.add(make_knob(phrasesim_name_m))
            .fallback("nwpr")
            .desc("Type of phrase sim model: [nwpf: n-gram idf-weighted precision/recall]")
            .name("psname")
            .name("phrasesim-type")
            ;
         p.add(make_knob(n_m))
            .fallback(0)
            .desc("N-gram size")
            .name("ngram-size")
            .name("n")
            ;
      }
   }; // struct phrasesim_options

   static std::map<std::string, std::map<std::string, std::pair<double, double> > > mpscache_m;
   static std::map<std::string, std::map<std::string, std::pair<double, double> > > xpscache_m;

   template <class opt_T>
   class phrasesim_t {
   public:
      typedef opt_T opt_type;

      phrasesim_t() {}

      phrasesim_t(opt_T opt) {
         lexsim_p = new lexsim_t(opt.lexsim_name_m, opt.outlexsim_path_m, opt.inplexsim_path_m);
         //std::cerr << "reflw_name\t" << opt.reflexweight_name_m << "\treflw_path\t" << opt.reflexweight_path_m
         //   << "\treflw_threshold\t" << opt.reflexweight_threshold_m << std::endl;
         reflexweight_p = new lexweight_t(opt.reflexweight_name_m, opt.reflexweight_path_m);
         if (opt.hyplexweight_name_m != "") {
            //std::cerr << "hyplw_name\t" << opt.hyplexweight_name_m << "\thyplw_path\t" << opt.hyplexweight_path_m
            //   << "\thyplw_threshold\t" << opt.hyplexweight_threshold_m << std::endl;
            hyplexweight_p = new lexweight_t(opt.hyplexweight_name_m, opt.hyplexweight_path_m);
         } else if (opt.reflexweight_name_m != "") {
            hyplexweight_p = reflexweight_p;
         }

         if (opt.inplexweight_name_m != "") {
            inplexweight_p = new lexweight_t(opt.inplexweight_name_m, opt.inplexweight_path_m);
         }

         phrasesim_name_m = opt.phrasesim_name_m;
         hyplexweight_name_m = opt.hyplexweight_name_m;
         inplexweight_name_m = opt.inplexweight_name_m;
         n_m = opt.n_m;
      }

      phrasesim_t(phrasesim_t& rhs) {
         lexsim_p = new lexsim_t(*(rhs.lexsim_p));
         reflexweight_p = new lexweight_t(*(rhs.reflexweight_p));
         hyplexweight_p = new lexweight_t(*(rhs.hyplexweight_p));
         inplexweight_p = new lexweight_t(*(rhs.inplexweight_p));
         hyplexweight_name_m = rhs.hyplexweight_name_m;
         inplexweight_name_m = rhs.inplexweight_name_m;
         phrasesim_name_m = rhs.phrasesim_name_m;
         n_m = rhs.n_m;
      }

      ~phrasesim_t() {
         if (lexsim_p != NULL) {
            delete lexsim_p;
            lexsim_p = NULL;
         }
         if (reflexweight_p != NULL) {
            delete reflexweight_p;
            reflexweight_p = NULL;
            if (hyplexweight_name_m == "") {
               hyplexweight_p = NULL;
            }
            if (inplexweight_name_m == "") {
               inplexweight_p = NULL;
            }
         }
         if (hyplexweight_p != NULL) {
            delete hyplexweight_p;
            hyplexweight_p = NULL;
         }
         if (inplexweight_p != NULL) {
            delete inplexweight_p;
            inplexweight_p = NULL;
         }
      }

      double get_lexweight(std::vector<std::string>& tokens, int mode) {
         double result = 0.0;
         for (auto it = tokens.begin(); it != tokens.end(); it++) {
            auto t = *it;
            if (mode == yisi::REF_MODE) {
               result += (*reflexweight_p)(t);
            } else if (mode == yisi::HYP_MODE) {
               result += (*hyplexweight_p)(t);
            } else {
               result += (*inplexweight_p)(t);
            }
         }
         return result;
      }

      std::pair<double, double> operator()(std::vector<std::string> s1tokens,
                                           std::vector<std::string>& hyptokens, int mode) {
         std::pair<double, double> result;
         if (s1tokens.size() == 0 || hyptokens.size() == 0) {
            result = std::make_pair(0.0, 0.0);
            return result;
         }
         std::string s1txt;
         size_t i;
         for (i = 0; i < s1tokens.size() - 1; i++) {
            s1txt = s1txt + s1tokens[i] + " ";
         }
         s1txt = s1txt + s1tokens[i];
         std::string hyptxt;
         size_t j;
         for (j = 0; j < hyptokens.size() - 1; j++) {
            hyptxt = hyptxt + hyptokens[j] + " ";
         }
         hyptxt = hyptxt + hyptokens[j];

         if (mode == yisi::INP_MODE) {
            if (xpscache_m.find(s1txt) != xpscache_m.end()) {
               if (xpscache_m[s1txt].find(hyptxt) != xpscache_m[s1txt].end()) {
                  return xpscache_m[s1txt][hyptxt];
               }
            } else {
               std::map<std::string, std::pair<double, double> > c;
               xpscache_m[s1txt] = c;
            }
         } else {
            if (mpscache_m.find(s1txt) != mpscache_m.end()) {
               if (mpscache_m[s1txt].find(hyptxt) != mpscache_m[s1txt].end()) {
                  return mpscache_m[s1txt][hyptxt];
               }
            } else {
               std::map<std::string, std::pair<double, double> > c;
               mpscache_m[s1txt] = c;
            }
         }
         auto s = nwpr(s1tokens, hyptokens, mode);
         if (mode == yisi::INP_MODE) {
            xpscache_m[s1txt][hyptxt] = s;
         } else {
            mpscache_m[s1txt][hyptxt] = s;
         }
         //std::cerr << "(" << s1txt << " ||| " << hyptxt << " ||| " << s.first << "," << s.second << ")" << std::endl;
         return s;
      };

      std::pair<double, double> ngram(std::vector<std::string>& s1tokens,
                                      std::vector<std::string>& hyptokens, int mode) {
         //std::cerr<<"ng: " << s1tokens.size()<<std::endl;
         //std::cerr<<"ng: " << hyptokens.size()<<std::endl;
         if (s1tokens.size() != hyptokens.size()) {
            std::cerr << "ERROR: Failed to compute n-gram similarity - "
                      << "s1 n-gram size != hyp n-gram size. Exiting..." << std::endl;
            exit(1);
         }
         double presult = 0.0;
         double rresult = 0.0;
         double plen = 0.0;
         double rlen = 0.0;
         for (size_t i = 0; i < s1tokens.size(); i++) {
            double rw = 0.0;
            double pw = 0.0;
            double ls = 0.0;
            if (mode == yisi::INP_MODE) {
               rw = (*inplexweight_p)(s1tokens[i]);
            } else if (mode == yisi::REF_MODE) {
               rw = (*reflexweight_p)(s1tokens[i]);
            }
            pw = (*hyplexweight_p)(hyptokens[i]);
            //std::cerr << s1tokens[i] << " ||| " << hyptokens[i];
            ls = lexsim_p->get_sim(s1tokens[i], hyptokens[i], mode);
            //std::cerr << ls << std::endl;
            rresult += rw * ls;
            presult += pw * ls;
            rlen += rw;
            plen += pw;
         }
         std::pair<double, double> result = std::make_pair(presult / plen, rresult / rlen);
         return result;
      }

      double ngramlw(std::vector<std::string>& tokens, int mode) {
         double result = 0.0;
         for (auto it = tokens.begin(); it != tokens.end(); it++) {
            if (mode == yisi::REF_MODE) {
               result += (*reflexweight_p)(*it);
            } else if (mode == yisi::HYP_MODE) {
               result += (*hyplexweight_p)(*it);
            } else {
               result += (*inplexweight_p)(*it);
            }
         }
         return result;
      }

      std::pair<double, double> nwpr(std::vector<std::string>& s1tokens,
                                     std::vector<std::string>& hyptokens, int mode) {
         std::string s1txt = yisi::join(s1tokens);
         std::string hyptxt = yisi::join(hyptokens);
         //std::cerr << s1txt << std::endl;
         //std::cerr<<hyptxt<<std::endl;

         std::vector<std::vector<std::string> > s1ngrams;
         std::vector<std::vector<std::string> > hypngrams;

         if ((int)s1tokens.size() < n_m || (int)hyptokens.size() < n_m) {
	   s1ngrams = yisi::collect_ngram(std::min(s1tokens.size(), hyptokens.size()), s1tokens);
           hypngrams = yisi::collect_ngram(std::min(s1tokens.size(), hyptokens.size()), hyptokens);
         } else {
           s1ngrams = yisi::collect_ngram(n_m, s1tokens);
           hypngrams = yisi::collect_ngram(n_m, hyptokens);
         }
         //std::cerr << s1ngrams.size() << std::endl;
         //std::cerr << hypngrams.size()<<std::endl;
         double nom = 0.0;
         double denom = 0.0;

         for (size_t ii = 0; ii < s1ngrams.size(); ii++) {
            std::string s1ngtxt = yisi::join(s1ngrams[ii]);

            double sim = 0.0;
            double rw = ngramlw(s1ngrams[ii], mode);

            for (size_t jj = 0; jj < hypngrams.size(); jj++) {
               std::string hypngtxt = yisi::join(hypngrams[jj]);
               //std::cerr << "ng sim of " << s1ngtxt << "," << hypngtxt << std::endl;
               sim = std::fmax(sim, ngram(s1ngrams[ii], hypngrams[jj], mode).second);
            }
            nom += rw * sim;
            denom += rw;
         }
         double recall = nom / denom;
         //std::cerr << "nnwr: " << recall << std::endl;
         //if (a >= 1) {
         //   return recall;
         //}

         nom = 0.0;
         denom = 0.0;
         //std::cerr<<hypngrams.size()<<std::endl;
         //std::cerr<<refngrams.size()<<std::endl;
         for (size_t iii = 0; iii < hypngrams.size(); iii++) {
            double hs = 0.0;
            double hw = ngramlw(hypngrams[iii], yisi::HYP_MODE);
            //std::cerr <<"here1"<<std::endl;
            for (size_t jjj = 0; jjj < s1ngrams.size(); jjj++) {
               //std::cerr<<"here2"<<std::endl;
               hs = std::fmax(hs, ngram(s1ngrams[jjj], hypngrams[iii], mode).first);
            }
            nom += hw * hs;
            denom += hw;
         }
         double precision = nom / denom;
         //std::cerr << "nnwp: " << precision << std::endl;
         //if (a<=0){
         //   return precision;
         //}
         //if ((a*precision+(1-a)*recall) > 0.0){
         //   return (precision*recall)/(a*precision+(1-a)*recall);
         //} else {
         //   return 0.0;
         //}
         std::pair<double, double> result = std::make_pair(precision, recall);
         return result;
      }

      double exact(std::vector<std::string>& reftokens, std::vector<std::string>& hyptokens) {
         if (reftokens.size() != hyptokens.size()) {
            std::cerr << "ERROR: Failed to compute n-gram precision - "
               << "ref n-gram size != hyp n-gram size. Exiting..." << std::endl;
            exit(1);
         }
         for (size_t i = 0; i < reftokens.size(); i++) {
            double ls = lexsim_p->get_sim(reftokens[i], hyptokens[i], yisi::REF_MODE);
            if (ls < 1.0) {
               return 0.0;
            }
         }
         return 1.0;
      }

      double mean(std::vector<std::string>& reftokens, std::vector<std::string>& hyptokens) {
         std::vector<double> ref;
         std::vector<double> hyp;
         //std::cerr << "Computing ref phrase vector mean ... ";
         for (auto rt = reftokens.begin(); rt != reftokens.end(); rt++) {
            auto refv = lexsim_p->get_wv(*rt, yisi::REF_MODE);
            if (ref.size() == 0) {
               if (refv.size() > 0) {
                  ref = refv;
               }
            } else {
               if (refv.size() == ref.size()) {
                  for (int i = 0; i < (int)ref.size(); i++) {
                     ref[i] += refv[i];
                  }
               }
            }
         }
         // turn into unit vector
         double rlen = 0.0;
         for (int i = 0; i < (int)ref.size(); i++) {
            rlen += ref[i] * ref[i];
         }
         rlen = sqrt(rlen);
         for (int i = 0; i < (int)ref.size(); i++) {
            ref[i] /= rlen;
         }
         //std::cerr << "Done" << std::endl;
         //std::cerr << "Computing hyp phrase vector mean ... ";
         for (auto ht = hyptokens.begin(); ht != hyptokens.end(); ht++) {
            auto hypv = lexsim_p->get_wv(*ht, yisi::HYP_MODE);
            if (hyp.size() == 0) {
               if (hypv.size() > 0) {
                  hyp = hypv;
               }
            } else {
               if (hypv.size() == hyp.size()) {
                  for (int i = 0; i < (int)hyp.size(); i++) {
                     hyp[i] += hypv[i];
                  }
               }
            }
         }
         // turn into unit vector
         double hlen = 0.0;
         for (int i = 0; i < (int)hyp.size(); i++) {
            hlen += hyp[i] * hyp[i];
         }
         hlen = sqrt(hlen);
         for (int i = 0; i < (int)hyp.size(); i++) {
            hyp[i] /= hlen;
         }

         //std::cerr << "Done" << std::endl;
         //std::cerr << "Computing vector sim ...";
         double result = lexsim_p->get_sim(ref, hyp);
         //std::cerr << "Done" << std::endl;
         return result;
      }

   private:
      lexsim_t* lexsim_p;
      lexweight_t* inplexweight_p;
      lexweight_t* reflexweight_p;
      lexweight_t* hyplexweight_p;
      std::string hyplexweight_name_m;
      std::string inplexweight_name_m;
      std::string phrasesim_name_m;
      int n_m;
   }; // class phrasesim_t

} // yisi

#endif
