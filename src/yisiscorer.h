/**
 * @file yisiscorer.h
 * @brief YiSi scorer
 *
 * @author Jackie Lo
 *
 * Class definition of YiSi scorer class:
 *    - yisiscorer_t 
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef YISISCORER_H
#define YISISCORER_H

#include "srlgraph.h"
#include "yisigraph.h"
#include "srl.h"
#include <string>
#include <vector> 
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

namespace yisi {

   struct yisi_options {
      std::string inpsrl_name_m;
      std::string inpsrl_path_m;
      std::string refsrl_name_m;
      std::string refsrl_path_m;
      std::string hypsrl_name_m;
      std::string hypsrl_path_m;

      std::string labelconfig_path_m;
      std::string weightconfig_path_m;
      std::string frameweight_name_m;

      double alpha_m;
      double beta_m;

      void init(com::masaers::cmdlp::parser& p) {
         using namespace com::masaers::cmdlp;

         p.add(make_knob(inpsrl_name_m))
	   .fallback("")
	   .desc("Type of input language SRL: [read|mate]")
	   .name("inpsrl-type")
	   ;
         p.add(make_knob(inpsrl_path_m))
	   .fallback("")
	   .desc("[read: path to assert formated parse of input sentences "
	         "| mate: full path and filename of <srclang>.mplsconfig]")
	   .name("inpsrl-path")
	   ;
         p.add(make_knob(hypsrl_name_m))
	   .fallback("")
	   .desc("Type of output language SRL: [read|mate]")
	   .name("outsrl-type")
	   .name("hypsrl-type")
	   .name("srl-type")
	   ;
         p.add(make_knob(hypsrl_path_m))
	   .fallback("")
	   .desc("[read: path to assert formatted parse output "
	         "| mate: full path and filename of <tgtlang>.mplsconfig]")
	   .name("outsrl-path")
	   .name("hypsrl-path")
	   .name("srl-path")
    ;
         p.add(make_knob(refsrl_name_m))
           .fallback("")
           .desc("Type of reference SRL (specify only if it is different from "
                 "the hypothesis SRL): [read|mate]")
           .name("refsrl-type")
           ;
         p.add(make_knob(refsrl_path_m))
           .fallback("")
           .desc("[read: path to assert formatted parse reference "
                 "| mate: full path and filename of <tgtlang>.mplsconfig]")
           .name("refsrl-path")
           ;
         p.add(make_knob(labelconfig_path_m))
	   .fallback("")
	   .desc("Path to YiSi SRL role label config file")
	   .name("labelconfig-path")
	   ;
         p.add(make_knob(weightconfig_path_m))
	   .fallback("")
	   .desc("Path to YiSi SRL role label config file (default: "
	         "<empty string> to use YiSi unsupervised estimation of weight")
	   .name("weightconfig-path")
	   ;
         p.add(make_knob(frameweight_name_m))
	   .fallback("coverage")
	   .desc("Type of frame weight function: [uniform|coverage(default)]")
	   .name("frameweight-type")
	   ;
         p.add(make_knob(beta_m))
	   .fallback(0.0)
	   .desc("Beta value of YiSi [0.0(default)]")
	   .name("beta")
	   ;
         p.add(make_knob(alpha_m))
	   .fallback(0.5)
	   .desc("Ratio of precision & recall in YiSi")
	   .name("alpha")
	   ;
      }
   }; // struct yisi_options

   template<class opt_T>
   class yisiscorer_t {
   public:
      typedef opt_T opt_type;

      yisiscorer_t() {}

      yisiscorer_t(opt_T opt) {
         alpha_m = opt.alpha_m;
         frameweight_name_m = opt.frameweight_name_m;
         alpha_m = opt.alpha_m;
         beta_m = opt.beta_m;

         int i = 0;
         if (opt.labelconfig_path_m != "") {
            std::cerr << "Reading labelconfig from " << opt.labelconfig_path_m << " ... ";
            std::ifstream LBL(opt.labelconfig_path_m.c_str());
            if (!LBL) {
               std::cerr << "ERROR: Failed to open labelconfig. Exiting..." << std::endl;
               exit(1);
            }
            while (!LBL.eof()) {
               std::string line;
               getline(LBL, line);
               if (line != "") {
                  std::istringstream iss(line);
                  while (!iss.eof()) {
                     std::string label;
                     iss >> label;
                     label_m[label] = i;
                  }
                  i++;
               }
            }
            LBL.close();
            std::cerr << "Done." << std::endl;
         }

         weightconfig_path_m = opt.weightconfig_path_m;
         if (weightconfig_path_m != ""
               && weightconfig_path_m != "lexweight"
               && weightconfig_path_m != "uniform") {
            std::cerr << "Reading weightconfig from " << opt.weightconfig_path_m << " ... ";
            std::ifstream W(weightconfig_path_m.c_str());
            if (!W) {
               std::cerr << "ERROR: Failed to open weightconfig. Exiting..." << std::endl;
               exit(1);
            }
            while (!W.eof()) {
               double w;
               W >> w;
               weight_m.push_back(w);
            }
            W.close();
            std::cerr << "Done." << std::endl;
            if ((int)weight_m.size() != i) {
               std::cerr << "ERROR: Number of weights in weightconfig does not match "
                  << "with number of lines in labelconfig. Exiting..." << std::endl;
               exit(1);
            }
         } else {
            for (int j = 0; j < i; j++) {
               weight_m.push_back(1.0);
            }
         }

         phrasesim_p = new phrasesim_t<opt_type>(opt);
         hypsrl_p = new srl_t(opt.hypsrl_name_m, opt.hypsrl_path_m);
         hypsrl_name_m = opt.hypsrl_name_m;
         if (opt.refsrl_name_m != "") {
            refsrl_p = new srl_t(opt.refsrl_name_m, opt.refsrl_path_m);
         } else {
            refsrl_p = hypsrl_p;;
         }
         refsrl_name_m = opt.refsrl_name_m;
         inpsrl_p = new srl_t(opt.inpsrl_name_m, opt.inpsrl_path_m);
         inpsrl_name_m = opt.inpsrl_name_m;
      } // yisiscorer_t

      ~yisiscorer_t() {
         if (phrasesim_p != NULL) {
            delete phrasesim_p;
            phrasesim_p = NULL;
         }
         if (inpsrl_p != NULL) {
            delete inpsrl_p;
            inpsrl_p = NULL;
         }
         if (hypsrl_p != NULL) {
            delete hypsrl_p;
            hypsrl_p = NULL;
            if (refsrl_name_m == "") {
               refsrl_p = NULL;
            }
         }
         if (refsrl_p != NULL) {
            delete refsrl_p;
            refsrl_p = NULL;
         }
      }

      void writecache() {
         phrasesim_p->writecache();
      }

      void readcache() {
         phrasesim_p->readcache();
      }

      void estimate_weight(std::vector<srlgraph_t> srls) {
         for (auto it = srls.begin(); it != srls.end(); it++) {
            auto preds = it->get_preds();
            for (auto jt = preds.begin(); jt != preds.end(); jt++) {
               auto pred_label = it->get_role_label(*jt);
               if (label_m.find(pred_label) == label_m.end()) {
                  std::cerr << "ERROR: Unknown predicate label '" << pred_label
                     << "'. Check your labelconfig. Exiting..." << std::endl;
                  exit(1);
               }
               weight_m[label_m[pred_label]] += 0.25;
               auto args = it->get_args(*jt);
               for (auto kt = args.begin(); kt != args.end(); kt++) {
                  auto arg_label = it->get_role_label(*kt);
                  if (label_m.find(arg_label) == label_m.end()) {
                     std::cerr << "ERROR: Unknown argument label '" << arg_label
                        << "'. Check your labelconfig. Exiting..." << std::endl;
                     exit(1);
                  }
                  weight_m[label_m[arg_label]] += 1.0;
               }
            }
         }
      }

      void estimate_weight(std::vector<std::vector<srlgraph_t> > msrls) {
         for (auto it = msrls.begin(); it != msrls.end(); it++) {
            estimate_weight(*it);
         }
      }

      std::vector<srlgraph_t> inpsrlparse(std::vector<sent_t*> inpsents) {
         //std::cerr << "Tokenizing/SRL-ing the input ...";
         std::vector<srlgraph_t> result = inpsrl_p->parse(inpsents);
         //std::cerr << "Done." << std::endl;
         if (weightconfig_path_m == "") {
            this->estimate_weight(result);
         }
         return result;
      }

      std::vector<srlgraph_t> refsrlparse(std::vector<sent_t*> refsents) {
         //std::cerr << "Tokenizing/SRL-ing the references ... ";
         std::vector<srlgraph_t> result = refsrl_p->parse(refsents);
         //std::cerr << "Done." << std::endl;
         if (weightconfig_path_m == "") {
            this->estimate_weight(result);
         }
         return result;
      }

      std::vector<srlgraph_t> hypsrlparse(std::vector<sent_t*> hypsents) {
         //std::cerr << "Tokenizing/SRL-ing the hypotheses ... ";
         std::vector<srlgraph_t> result = hypsrl_p->parse(hypsents);
         //std::cerr << "Done." << std::endl;
         return result;
      }

      srlgraph_t hypsrlparse(sent_t* hypsent) {
         //std::cerr <<"Tokenizing/SRL-ing the hypothesis ... ";
         srlgraph_t result = hypsrl_p->parse(hypsent);
         //std::cerr << "Done." << std::endl;
         return result;
      }

      yisigraph_t align(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph) {
         //std::cerr << "Creating YiSi graph ... ";
         yisigraph_t result(refsrlgraph, hypsrlgraph);
         //std::cerr << "start aligning ... ";
         result.align(phrasesim_p);
         //result.print(std::cerr);
         //std::cerr << "Done." << std::endl;
         return result;
      }

      yisigraph_t align(const std::vector<srlgraph_t> refsrlgraph,
                        const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph) {
         //std::cerr << "Creating YiSi graph with input... ";
         yisigraph_t result(refsrlgraph, hypsrlgraph, inpsrlgraph);
         //std::cerr << "start aligning ... ";
         result.align(phrasesim_p);
         //result.print(std::cerr);
         //std::cerr << "Done." << std::endl;
         return result;
      };

      double score(yisigraph_t& yg) {
         double precision = score(yg, yisi::HYP_MODE);
         double recall = score(yg, yisi::REF_MODE);
         double yisi = 0.0;
         if (precision == 0.0 || recall == 0.0) {
            yisi = 0.0;
         } else {
            yisi = (precision * recall) / (alpha_m * precision + (1.0 - alpha_m) * recall);
         }
         return yisi;
         //double flat = yg.get_sentsim();
         //if (mode_m == "flat") {
         //   return flat;
         //} else {
         //   //std::cerr<<"Computing YiSi precision ... ";
         //   double precision = score(yg, yisi::HYP_MODE);
         //   //std::cerr<<"Done."<<std::endl;
         //   //std::cerr<<"Computing YiSi recall ... ";
         //   double recall = score(yg, yisi::REF_MODE);
         //   //std::cerr<<"Done."<<std::endl;
         //   double yisi = 0.0;
         //   if (precision == 0.0 || recall == 0.0) {
         //      yisi = 0.0;
         //   } else {
         //      yisi = (precision * recall)
         //             / (alpha_m * precision + (1.0 - alpha_m) * recall);

         //      if (prfunc_name_m == "f" || prfunc_name_m == "lexexp") {
         //         yisi = (precision * recall)
         //                / (alpha_m * precision + (1.0 - alpha_m) * recall);
         //      } else if (prfunc_name_m == "max") {
         //         yisi = std::max(precision, recall);
         //      } else {
         //         std::cerr
         //            << "ERROR: unknown precision/recall agg function name. Exiting ..."
         //            << std::endl;
         //         exit(1);
         //      }
         //   }
         //   if (mode_m == "yisi" || mode_m == "yisi_flat"
         //      || mode_m == "features") {
         //      return yisi;
         //   } else if (mode_m == "yisi+float") {
         //      return (yisi + flat) / 2.0;
         //   } else {
         //      double w = std::atof(mode_m.c_str());
         //      return w * yisi + (1 - w) * flat;
         //   }
         //}
      }

      std::vector<double> features(yisigraph_t& yg) {
         std::vector<double> result;
         //double flat =  yg.get_sentsim();
         //result.push_back(flat);
         //result.push_back(score(yg));
         std::vector<double> precision = features(yg, yisi::HYP_MODE);
         std::vector<double> recall = features(yg, yisi::REF_MODE);
         for (auto it = precision.begin(); it != precision.end(); it++) {
            result.push_back(*it);
         }
         for (auto it = recall.begin(); it != recall.end(); it++) {
            result.push_back(*it);
         }
         return result;
      }

   private:
      double score(yisigraph_t yg, int mode) {
         //std::cerr <<"Scoring...";
         auto f = features(yg, mode);
         double structure = f[weight_m.size()];
         double flat = f[weight_m.size() + 1];
         //std::cerr <<"(" << beta_m <<"," <<structure <<"," <<flat <<")";
         //std::cerr <<"Done."<<std::endl;
         return beta_m * structure + (1 - beta_m) * flat;

         //double nom = 0.0;
         //double denom = 0.0;
         //if (yg.get_sentlength(mode) == 0.0) {
         //   return 0.0;
         //}
         //if (mode_m == "yisi_flat") {
         //   if (frameweight_name_m == "coverage") {
         //      nom += yg.get_sentlength(mode) * yg.get_sentsim();
         //      denom += yg.get_sentlength(mode);
         //   } else {
         //      nom += yg.get_sentsim();
         //      denom += 1;
         //   }
         //}

         //auto preds = yg.get_preds(mode);

         //for (auto it = preds.begin(); it != preds.end(); it++) {
         //   auto predid = *it;
         //   double sanity_check = yg.get_rolespanlength(predid, mode);
         //   double predsim = yg.get_alignsim(predid, mode);
         //   auto predlabel = yg.get_rolelabel(predid, mode);
         //   double predweight = get_roleweight(yg, predid, mode);

         //   if (sanity_check > 0) {
         //      // if (prfunc_name_m=="f" || prfunc_name_m=="max"){
         //      double fw = yg.get_rolespanlength(predid, mode);
         //      double fn = 0.0;
         //      if (predsim >= rolesim_threshold_m) {
         //         fn = predweight * predsim;
         //      }
         //      double fd = predweight;
         //      auto args = yg.get_args(predid, mode);
         //      for (auto jt = args.begin(); jt != args.end(); jt++) {
         //         auto argid = *jt;
         //         fw += yg.get_rolespanlength(argid, mode);

         //         auto arglabel = yg.get_rolelabel(argid, mode);
         //         auto alignlabel = yg.get_alignlabel(argid, mode);
         //         double argsim = yg.get_alignsim(argid, mode);
         //         double argweight = get_roleweight(yg, argid, mode);
         //         if (argsim >= rolesim_threshold_m
         //            && match(arglabel, alignlabel)) {
         //            fn += argweight * argsim;
         //         }
         //         fd += argweight;
         //      }
         //      if (fn > 0 && fd > 0) {
         //         if (frameweight_name_m == "coverage") {
         //            nom += fw * (fn / fd);
         //         } else {
         //            nom += fn / fd;
         //         }
         //      }
         //      if (frameweight_name_m == "coverage") {
         //         denom += fw;
         //      } else {
         //         denom += 1;
         //      }
         //   } else {
         //      if (predsim >= rolesim_threshold_m) {
         //         nom = predweight * predsim;
         //      }
         //      denom += predweight;
         //      auto args = yg.get_args(predid, mode);
         //      for (auto jt = args.begin(); jt != args.end(); jt++) {
         //         auto argid = *jt;
         //         auto arglabel = yg.get_rolelabel(argid, mode);
         //         auto alignlabel = yg.get_alignlabel(argid, mode);
         //         double argsim = yg.get_alignsim(argid, mode);
         //         double argweight = get_roleweight(yg, argid, mode);
         //         if (argsim >= rolesim_threshold_m
         //            && match(arglabel, alignlabel)) {
         //            nom += argweight * argsim;
         //         }
         //         denom += argweight;
         //      }

         //   }

         //}
         //}
         //if (nom > 0 && denom > 0) {
         //   return nom/denom;
         //} else {
         //   return 0.0;
         //}
      }

      std::vector<double> features(yisigraph_t yg, int mode) {
         if (mode == yisi::REF_MODE) {
            return rfeatures(yg);
         } else {
            return pfeatures(yg);
         }
      }

      void compute_features(yisigraph_t yg, std::vector<double> feats,
         double& structure, double& flat, int mode, int refid = -1) {
         flat = yg.get_sentsim(mode, refid);

         double tfw = 0.0; // total frame weight
         //std::vector<double> tsim(weight_m.size(), 0.0); // total similarity by role type
         //std::vector<double> tcount(weight_m.size(), 0.0); // total count by role type
         double nom = 0.0;
         double denom = 0.0;

         auto preds = yg.get_preds(mode, refid);

         for (auto it = preds.begin(); it != preds.end(); it++) {
            std::vector<double> sim(weight_m.size(), 0.0);
            std::vector<double> count(weight_m.size(), 0.0);
            auto predid = *it;
            double sanity_check = yg.get_rolespanlength(predid, mode, refid);
            double predsim = yg.get_alignsim(predid, mode, refid);
            auto predlabel = yg.get_rolelabel(predid, mode, refid);
            double predweight = get_roleweight(yg, predid, mode, refid);

            if (sanity_check > 0) {
               //if (prfunc_name_m=="f" || prfunc_name_m=="max"){
               double fw = yg.get_rolespanlength(predid, mode, refid);
               double fn = 0.0;

               sim[label_m[predlabel]] += predsim;
               fn = predweight * predsim;

               double fd = predweight;
               count[label_m[predlabel]] += 1.0;

               auto args = yg.get_args(predid, mode, refid);
               for (auto jt = args.begin(); jt != args.end(); jt++) {
                  auto argid = *jt;
                  fw += yg.get_rolespanlength(argid, mode, refid);

                  auto arglabel = yg.get_rolelabel(argid, mode, refid);
                  double argsim = 0.0;
                  yisigraph_t::label_type alignlabel;
                  if (mode == yisi::HYP_MODE) {
                     auto alignment = yg.get_hypalignment(argid);
                     for (auto it = alignment.begin(); it != alignment.end(); it++) {
                        double s = (it->second).second;
                        int id = it->first;
                        yisigraph_t::label_type l;
                        if (id < (int)yg.get_refsize()) {
                           l = yg.get_rolelabel((it->second).first, yisi::REF_MODE, id);
                        } else {
                           l = yg.get_rolelabel((it->second).first, yisi::INP_MODE);
                        }
                        if (s > argsim && match(arglabel, l)) {
                           argsim = s;
                           alignlabel = l;
                        }
                     }
                  } else {
                     alignlabel = yg.get_alignlabel(argid, mode, refid);
                     argsim = yg.get_alignsim(argid, mode, refid);
                  }

                  double argweight = get_roleweight(yg, argid, mode, refid);

                  sim[label_m[arglabel]] += argsim;
                  fn += argweight * argsim;

                  count[label_m[arglabel]] += 1.0;
                  fd += argweight;
               }

               if (fn > 0 && fd > 0) {
                  if (frameweight_name_m == "coverage") {
                     nom += fw * (fn / fd);
                  } else {
                     nom += fn / fd;
                  }
               }
               if (frameweight_name_m == "coverage") {
                  denom += fw;
               } else {
                  denom += 1;
               }

               for (size_t i = 0; i < feats.size(); i++) {
                  if (count[i] > 0) {
                     feats[i] += fw * (sim[i] / count[i]);
                  }
               }
               tfw += fw;
            }
         }
         if (tfw > 0) {
            for (size_t i = 0; i < feats.size(); i++) {
               feats[i] /= tfw;
            }
         }

         //if (prfunc_name_m == "lexexp") {
         //   for (size_t i = 0; i < tsim.size(); i++) {
         //      if (tcount[i] > 0) {
         //         result[i] = tsim[i] / tcount[i];
         //      }
         //   }
         //}
         if (nom > 0 && denom > 0) {
            structure = nom / denom;
         }
      }

      std::vector<double> pfeatures(yisigraph_t yg) {
         std::vector<double> result(weight_m.size(), 0.0);
         double structure = 0.0;
         double flat = 0.0;

         compute_features(yg, result, structure, flat, yisi::HYP_MODE);

         result.push_back(structure);
         result.push_back(flat);
         return result;
      }

      std::vector<double> rfeatures(yisigraph_t yg) {
         std::vector<double> result(weight_m.size(), 0.0);
         double mflat = 0.0;
         double mstructure = 0.0;

         //for all reference
         for (size_t i = 0; i < yg.get_refsize(); i++) {
            std::vector<double> feats(weight_m.size(), 0.0);
            double structure = 0.0;
            double flat = 0.0;
            //std::cerr << "Computing recall features for reference #" << i << " ... ";
            compute_features(yg, feats, structure, flat, yisi::REF_MODE, i);
            //std::cerr << "Done." << std::endl;
            if (structure > mstructure) {
               mstructure = structure;
               result = feats;
            }
            if (flat > mflat) {
               mflat = flat;
            }
         }

         //input
         if (yg.withinp()) {
            std::vector<double> feats(weight_m.size(), 0.0);
            double structure = 0.0;
            double flat = 0.0;
            //std::cerr << "Computing recall features for input ... ";
            compute_features(yg, feats, structure, flat, yisi::INP_MODE);
            //std::cerr << "Done." << std::endl;
            if (structure > mstructure) {
               mstructure = structure;
               result = feats;
            }
            if (flat > mflat) {
               mflat = flat;
            }
         }

         result.push_back(mstructure);
         result.push_back(mflat);
         return result;
      }

      bool match(std::string label1, std::string label2) {
         if (label1 == "U" || label2 == "U") {
            return false;
         } else {
            if (label_m.find(label1) == label_m.end()) {
               std::cerr << "ERROR: Unknown srl label '" << label1 << "' in YiSi for matching label 1. "
                  << "Check your labelconfig. Exiting..." << std::endl;
               exit(1);
            }
            if (label_m.find(label2) == label_m.end()) {
               std::cerr << "ERROR: unknown srl label '" << label2 << "' in yisi for matching label 2. "
                  << "Check your labelconfig. Exiting..." << std::endl;
               exit(1);
            }
            return (label_m[label1] == label_m[label2]);
         }
      }

      double get_roleweight(yisigraph_t yg, size_t roleid, int mode, int refid = -1) {
         if (weightconfig_path_m == "lexweight") {
            auto fillers = yg.get_role_filler_units(roleid, mode, refid);
            return phrasesim_p->get_lexweight(fillers, mode);
         } else {
            std::string label = yg.get_rolelabel(roleid, mode, refid);
            if (label_m.find(label) == label_m.end()) {
               std::cerr << "ERROR: Unknown srl label '" << label << "' in yisi for get_weight. "
                  << "Check your labelconfig. Exiting..." << std::endl;
               exit(1);
            }
            return weight_m[label_m[label]];
         }
      }

      phrasesim_t<opt_T>* phrasesim_p;
      srl_t* inpsrl_p;
      srl_t* refsrl_p;
      srl_t* hypsrl_p;

      std::string hypsrl_name_m;
      std::string refsrl_name_m;
      std::string inpsrl_name_m;
      std::string weightconfig_path_m;
      //std::string predweight_name_m;
      std::string frameweight_name_m;
      //std::string prfunc_name_m;

      std::map<std::string, int> label_m;
      std::vector<double> weight_m;
      double alpha_m;
      double beta_m;
   }; // class yisiscorer_t
  
} // yisi

#endif
