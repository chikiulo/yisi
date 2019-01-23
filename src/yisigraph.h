/**
 * @file yisigraph.h
 * @brief YiSi graph
 *
 * @author Jackie Lo
 *
 * Class definition of one class:
 *    - yisigraph_t
 * and the function definition for the YiSi alignment algorithm.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef YISIGRAPH_H
#define YISIGRAPH_H

#include "srlgraph.h"
#include "phrasesim.h"
#include "maxmatching.h"

#include <utility>
#include <string>
#include <vector> 
#include <iostream>

namespace yisi {

   class yisigraph_t{
   public:
      typedef srlgraph_t::span_type span_type;
      typedef srlgraph_t::label_type label_type;
      typedef srlgraph_t::srlnode_type srlnode_type;
      typedef srlgraph_t::srledge_type srledge_type;
      typedef srlgraph_t::srlnid_type srlnid_type;
      typedef srlgraph_t::srleid_type srleid_type;
      typedef std::pair<srlnid_type, double> alignment_type;

      yisigraph_t() {}
      yisigraph_t(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph);
      yisigraph_t(const std::vector<srlgraph_t> refsrlgraph, const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph);
      yisigraph_t(const yisigraph_t& rhs);
      void operator = (const yisigraph_t& rhs);

      template <typename T> void align(phrasesim_t<T>* phrasesim);

      bool withinp();
      size_t get_refsize();
      double get_sentlength(int mode, int refid=-1);
      double get_sentsim(int mode, int refid=-1);
      std::vector<srlnid_type> get_preds(int mode, int refid=-1);
      std::vector<srlnid_type> get_args(srlnid_type roleid, int mode, int refid=-1);
      std::vector<std::string>& get_sentence(int mode, int refid=-1);
      std::vector<std::string> get_role_fillers(srlnid_type roleid, int mode, int refid=-1);
      double get_rolespanlength(srlnid_type roleid, int mode, int refid=-1);
      label_type get_rolelabel(srlnid_type roleid, int mode, int refid=-1);
      std::vector<std::pair<int, alignment_type> > get_hypalignment(srlnid_type roleid);
      double get_alignsim(srlnid_type roleid, int mode, int refid=-1);
      label_type get_alignlabel(srlnid_type roleid, int mode, int refid=-1);

      void print(std::ostream& os);

   private:
      double spanlength(span_type span);
      std::vector<srlgraph_t> refsrlgraph_m;
      srlgraph_t hypsrlgraph_m;
      srlgraph_t inpsrlgraph_m;

      std::vector<std::map<srlnid_type, alignment_type> > refalignment_m;
      std::map<srlnid_type, std::vector<std::pair<int, alignment_type> > > hypalignment_m;
      std::map<srlnid_type, alignment_type> inpalignment_m;
      bool inp_b;
   }; // class yisigraph_t

   template <typename T>
   void yisigraph_t::align(phrasesim_t<T>* phrasesim) {
      //yisi alignment algorithm goes here
      //loop all references and input
      for (size_t refid = 0; refid < refsrlgraph_m.size(); refid++) {
         //std::cerr << "first align the sentence node" << std::endl;
         auto r = refsrlgraph_m[refid].get_sentence();
         //std::cerr << "Got r " << r.size() << std::endl;
         auto h = hypsrlgraph_m.get_sentence();
         //std::cerr << "Got h " << h.size() << std::endl;
         std::pair<double, double> sentsim = (*phrasesim)(r, h, yisi::REF_MODE);
         //std::cerr << "sentsim = (" << sentsim.first << "," << sentsim.second << ")";
         auto refroot = refsrlgraph_m[refid].get_root();
         //std::cerr << "refroot = " << refroot << std::endl;
         auto hyproot = hypsrlgraph_m.get_root();
         //std::cerr << "hyproot = " << hyproot << std::endl;
         refalignment_m.push_back(std::map<srlnid_type, alignment_type>());
         //std::cerr << "Done creating refalignment map" << std::endl;
         refalignment_m[refid][refroot] = alignment_type(hyproot, sentsim.second);
         //std::cerr << "Done adding ref alignment edge" << std::endl;
         if (hypalignment_m.find(hyproot) == hypalignment_m.end()) {
            hypalignment_m[hyproot] = std::vector<std::pair<int, alignment_type> >();
         }
         hypalignment_m[hyproot].push_back(std::make_pair(refid, alignment_type(refroot, sentsim.first)));
         //std::cerr << "Done adding hyp alignment edge" << std::endl;
         //std::cerr << "prepare the pred maxmatching matrix"<<std::endl;
         auto refpreds = refsrlgraph_m[refid].get_preds();
         auto hyppreds = hypsrlgraph_m.get_preds();
         maxmatching_t refpredmatch;
         maxmatching_t hyppredmatch;
         for (auto it = refpreds.begin(); it != refpreds.end(); it++) {
            auto refpredid = *it;
            auto refpredspan = refsrlgraph_m[refid].get_role_span(refpredid);
            if (refpredspan.first != refpredspan.second) {
               auto refpredphrase = refsrlgraph_m[refid].get_role_fillers(refpredid);
               for (auto jt = hyppreds.begin(); jt != hyppreds.end(); jt++) {
                  auto hyppredid = *jt;
                  auto hyppredspan = hypsrlgraph_m.get_role_span(hyppredid);
                  if (hyppredspan.first != hyppredspan.second) {
                     auto hyppredphrase = hypsrlgraph_m.get_role_fillers(hyppredid);
                     std::pair<double, double> predsim =
                        (*phrasesim)(refpredphrase, hyppredphrase, yisi::REF_MODE);
                     refpredmatch.add_weight(refpredid, hyppredid, predsim.second);
                     hyppredmatch.add_weight(refpredid, hyppredid, predsim.first);
                  }
               } // for jt
            }
         }  // for it
         //std::cerr <<"run maxmatch on preds" <<std::endl;
         auto rpr = refpredmatch.run();
         auto hpr = hyppredmatch.run();
         //std::cerr <<"for each aligned pred" <<std::endl;
         for (size_t i = 0; i < rpr.size(); i++) {
            auto aligned_ref_pred = rpr[i].first.first;
            auto aligned_hyp_pred = rpr[i].first.second;
            auto psim = rpr[i].second;
            refalignment_m[refid][aligned_ref_pred] = alignment_type(aligned_hyp_pred, psim);
            //hypalignment_m[aligned_hyp_pred] = alignment_type(aligned_ref_pred, psim);
            //std::cerr<<"prepare the arg maxmatching matrix"<<std::endl;
            auto refargs = refsrlgraph_m[refid].get_args(aligned_ref_pred);
            auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
            maxmatching_t argmatch;
            for (auto it = refargs.begin(); it != refargs.end(); it++) {
               auto refargid = *it;
               auto refargphrase = refsrlgraph_m[refid].get_role_fillers(refargid);
               for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
                  auto hypargid = *jt;
                  auto hypargphrase = hypsrlgraph_m.get_role_fillers(hypargid);
                  std::pair<double, double> argsim =
                     (*phrasesim)(refargphrase, hypargphrase, yisi::REF_MODE);
                  argmatch.add_weight(refargid, hypargid, argsim.second);
               } // for jt
            } // for it
            //std::cerr << "run maxmatch on args" <<std::endl;
            auto ar = argmatch.run();
            //std::cerr << "for each aligned arg" <<std::endl;
            for (size_t j = 0; j < ar.size(); j++) {
               auto aligned_ref_arg = ar[j].first.first;
               auto aligned_hyp_arg = ar[j].first.second;
               auto asim = ar[j].second;
               refalignment_m[refid][aligned_ref_arg] = alignment_type(aligned_hyp_arg, asim);
               //hypalignment_m[aligned_hyp_arg] = alignment_type(aligned_ref_arg, asim);
            } // for j
         } // for i
         for (size_t i = 0; i < hpr.size(); i++) {
            auto aligned_ref_pred = hpr[i].first.first;
            auto aligned_hyp_pred = hpr[i].first.second;
            auto psim = hpr[i].second;
            if (hypalignment_m.find(aligned_hyp_pred) == hypalignment_m.end()) {
               hypalignment_m[aligned_hyp_pred] =
                                 std::vector<std::pair<int, alignment_type> >();
            }
            hypalignment_m[aligned_hyp_pred].push_back(std::make_pair(refid,
                                       alignment_type(aligned_ref_pred, psim)));
            auto refargs = refsrlgraph_m[refid].get_args(aligned_ref_pred);
            auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
            maxmatching_t argmatch;
            for (auto it = refargs.begin(); it != refargs.end(); it++) {
               auto refargid = *it;
               auto refargphrase = refsrlgraph_m[refid].get_role_fillers(refargid);
               for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
                  auto hypargid = *jt;
                  auto hypargphrase = hypsrlgraph_m.get_role_fillers(hypargid);
                  std::pair<double, double> argsim =
                     (*phrasesim)(refargphrase, hypargphrase, yisi::REF_MODE);
                  argmatch.add_weight(refargid, hypargid, argsim.first);
               } // for jt
            } // for it
            auto ar = argmatch.run();
            for (size_t j = 0; j < ar.size(); j++) {
               auto aligned_ref_arg = ar[j].first.first;
               auto aligned_hyp_arg = ar[j].first.second;
               auto asim = ar[j].second;
               if (hypalignment_m.find(aligned_hyp_arg) == hypalignment_m.end()) {
                  hypalignment_m[aligned_hyp_arg] =
                                 std::vector<std::pair<int, alignment_type> >();
               }
               hypalignment_m[aligned_hyp_arg].push_back(std::make_pair(refid,
                                          alignment_type(aligned_ref_arg, asim)));
            }  // for j
         } // for i
      } // for refid
      //input
      if (inp_b) {
         auto r = inpsrlgraph_m.get_sentence();
         auto h = hypsrlgraph_m.get_sentence();
         std::pair<double, double> sentsim = (*phrasesim)(r, h, yisi::INP_MODE);
         auto inproot = inpsrlgraph_m.get_root();
         auto hyproot = hypsrlgraph_m.get_root();
         inpalignment_m[inproot] = alignment_type(hyproot, sentsim.second);
         if (hypalignment_m.find(hyproot) == hypalignment_m.end()) {
            hypalignment_m[hyproot] = std::vector<std::pair<int, alignment_type> >();
         }
         hypalignment_m[hyproot].push_back(std::make_pair((int)refsrlgraph_m.size(),
                                          alignment_type(inproot, sentsim.first)));
         auto inppreds = inpsrlgraph_m.get_preds();
         auto hyppreds = hypsrlgraph_m.get_preds();
         maxmatching_t inppredmatch;
         maxmatching_t hyppredmatch;
         for (auto it = inppreds.begin(); it != inppreds.end(); it++) {
            auto inppredid = *it;
            auto inppredspan = inpsrlgraph_m.get_role_span(inppredid);
            if (inppredspan.first != inppredspan.second) {
               auto inppredphrase = inpsrlgraph_m.get_role_fillers(inppredid);
               for (auto jt = hyppreds.begin(); jt != hyppreds.end(); jt++) {
                  auto hyppredid = *jt;
                  auto hyppredspan = hypsrlgraph_m.get_role_span(hyppredid);
                  if (hyppredspan.first != hyppredspan.second) {
                     auto hyppredphrase = hypsrlgraph_m.get_role_fillers(hyppredid);
                     std::pair<double, double> predsim =
                        (*phrasesim)(inppredphrase, hyppredphrase, yisi::INP_MODE);
                     inppredmatch.add_weight(inppredid, hyppredid, predsim.second);
                     hyppredmatch.add_weight(inppredid, hyppredid, predsim.first);
                  }
               }
            }
         }
         auto ipr = inppredmatch.run();
         auto hpr = hyppredmatch.run();
         for (size_t i = 0; i < ipr.size(); i++) {
            auto aligned_inp_pred = ipr[i].first.first;
            auto aligned_hyp_pred = ipr[i].first.second;
            auto psim = ipr[i].second;
            inpalignment_m[aligned_inp_pred] = alignment_type(aligned_hyp_pred, psim);
            auto inpargs = inpsrlgraph_m.get_args(aligned_inp_pred);
            auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
            maxmatching_t argmatch;
            for (auto it = inpargs.begin(); it != inpargs.end(); it++) {
               auto inpargid = *it;
               auto inpargphrase = inpsrlgraph_m.get_role_fillers(inpargid);
               for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
                  auto hypargid = *jt;
                  auto hypargphrase = hypsrlgraph_m.get_role_fillers(hypargid);
                  std::pair<double, double> argsim =
                     (*phrasesim)(inpargphrase, hypargphrase, yisi::INP_MODE);
                  argmatch.add_weight(inpargid, hypargid, argsim.second);
               }
            }
            auto ar = argmatch.run();
            for (size_t j = 0; j < ar.size(); j++) {
               auto aligned_inp_arg = ar[j].first.first;
               auto aligned_hyp_arg = ar[j].first.second;
               auto asim = ar[j].second;
               inpalignment_m[aligned_inp_arg] = alignment_type(aligned_hyp_arg, asim);
            }
         }
         for (size_t i = 0; i < hpr.size(); i++) {
            auto aligned_inp_pred = hpr[i].first.first;
            auto aligned_hyp_pred = hpr[i].first.second;
            auto psim = hpr[i].second;
            if (hypalignment_m.find(aligned_hyp_pred) == hypalignment_m.end()) {
               hypalignment_m[aligned_hyp_pred] =
                                 std::vector<std::pair<int, alignment_type> >();
            }
            hypalignment_m[aligned_hyp_pred].push_back(std::make_pair((int)refsrlgraph_m.size(),
                                                alignment_type(aligned_inp_pred, psim)));
            auto inpargs = inpsrlgraph_m.get_args(aligned_inp_pred);
            auto hypargs = hypsrlgraph_m.get_args(aligned_hyp_pred);
            maxmatching_t argmatch;
            for (auto it = inpargs.begin(); it != inpargs.end(); it++) {
               auto inpargid = *it;
               auto inpargphrase = inpsrlgraph_m.get_role_fillers(inpargid);
               for (auto jt = hypargs.begin(); jt != hypargs.end(); jt++) {
                  auto hypargid = *jt;
                  auto hypargphrase = hypsrlgraph_m.get_role_fillers(hypargid);
                  std::pair<double, double> argsim =
                     (*phrasesim)(inpargphrase, hypargphrase, yisi::INP_MODE);
                  argmatch.add_weight(inpargid, hypargid, argsim.first);
               }
            }
            auto ar = argmatch.run();
            for (size_t j = 0; j < ar.size(); j++) {
               auto aligned_inp_arg = ar[j].first.first;
               auto aligned_hyp_arg = ar[j].first.second;
               auto asim = ar[j].second;
               if (hypalignment_m.find(aligned_hyp_arg) == hypalignment_m.end()) {
                  hypalignment_m[aligned_hyp_arg] =
                                 std::vector<std::pair<int, alignment_type> >();
               }
               hypalignment_m[aligned_hyp_arg].push_back(std::make_pair((int)refsrlgraph_m.size(),
                                                alignment_type(aligned_inp_arg, asim)));
            }
         }
      }
   } // align

   std::ostream& operator<<(std::ostream& os, const yisi::yisigraph_t& m);

} // yisi



#endif
