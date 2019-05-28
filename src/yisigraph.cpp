/**
 * @file yisigraph.cpp
 * @brief YiSi graph
 *
 * @author Jackie Lo
 *
 * Class implementation of the class:
 *    - yisigraph_t
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "yisigraph.h"

#include <fstream>
#include <sstream>

using namespace yisi;
using namespace std;

yisigraph_t::yisigraph_t(const vector<srlgraph_t> refsrlgraph, 
			 const srlgraph_t hypsrlgraph) {
   refsrlgraph_m = refsrlgraph;
   hypsrlgraph_m = hypsrlgraph;
   inp_b = false;

   //cout << endl;
   //cout << "refsrlgraph:" << endl;
   //cout << refsrlgraph_m;
   //cout << "hypsrlgraph:" << endl;
   //cout << hypsrlgraph_m;
   //cout<<"Done."<<endl;   
}

yisigraph_t::yisigraph_t(const vector<srlgraph_t> refsrlgraph,
                         const srlgraph_t hypsrlgraph, const srlgraph_t inpsrlgraph) {
   refsrlgraph_m = refsrlgraph;
   hypsrlgraph_m = hypsrlgraph;
   inpsrlgraph_m = inpsrlgraph;
   inp_b = true;

   //cout << endl;
   //cout << "refsrlgraph:" << endl;
   //cout << refsrlgraph_m;
   //cout << "hypsrlgraph:" << endl;
   //cout << hypsrlgraph_m;
   //cout<<"Done."<<endl;
}

yisigraph_t::yisigraph_t(const yisigraph_t& rhs) {
   refsrlgraph_m = rhs.refsrlgraph_m;
   hypsrlgraph_m = rhs.hypsrlgraph_m;
   refalignment_m = rhs.refalignment_m;
   hypalignment_m = rhs.hypalignment_m;
   inp_b = rhs.inp_b;
   if (rhs.inp_b) {
      inpsrlgraph_m = rhs.inpsrlgraph_m;
      inpalignment_m = rhs.inpalignment_m;
   }
}

void yisigraph_t::operator=(const yisigraph_t& rhs) {
   refsrlgraph_m = rhs.refsrlgraph_m;
   hypsrlgraph_m = rhs.hypsrlgraph_m;
   refalignment_m = rhs.refalignment_m;
   hypalignment_m = rhs.hypalignment_m;
   inp_b = rhs.inp_b;
   if (rhs.inp_b) {
      inpsrlgraph_m = rhs.inpsrlgraph_m;
      inpalignment_m = rhs.inpalignment_m;
   }
}

bool yisigraph_t::withinp() {
   return inp_b;
}

size_t yisigraph_t::get_refsize() {
   return refsrlgraph_m.size();
}

/*
double yisigraph_t::get_sentlength(int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return spanlength(inpsrlgraph_m.get_role_span(inpsrlgraph_m.get_root()));
         } else {
            return 0.0;
         }
         break;
      case yisi::HYP_MODE:
         return spanlength(hypsrlgraph_m.get_role_span(hypsrlgraph_m.get_root()));
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return spanlength(refsrlgraph_m[refid].get_role_span(refsrlgraph_m[refid].get_root()));
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0,"
                 << refsrlgraph_m.size() << "]. Sent length undefined. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
	cerr << "ERROR: Unknown mode in sent length. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}
*/

double yisigraph_t::get_sentsim(int mode, int refid) {
   double result = 0.0;

   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return inpalignment_m[inpsrlgraph_m.get_root()].second;
         } else {
            return 0.0;
         }
         break;
      case yisi::HYP_MODE:
         for (auto it = hypalignment_m[hypsrlgraph_m.get_root()].begin();
               it != hypalignment_m[hypsrlgraph_m.get_root()].end(); it++) {
            double s = (it->second).second;
            if (s > result) {
               result = s;
            }
         }
         return result;
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return refalignment_m[refid][refsrlgraph_m[refid].get_root()].second;
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0,"
                 << refsrlgraph_m.size() << "]. Sent sim undefined. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in sent sim. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

vector<yisigraph_t::srlnid_type> yisigraph_t::get_preds(int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return inpsrlgraph_m.get_preds();
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input predicates. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         return hypsrlgraph_m.get_preds();
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return refsrlgraph_m[refid].get_preds();
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference predicates. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get preds. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

vector<yisigraph_t::srlnid_type> yisigraph_t::get_args(srlnid_type roleid, int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return inpsrlgraph_m.get_args(roleid);
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input roles. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         return hypsrlgraph_m.get_args(roleid);
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return refsrlgraph_m[refid].get_args(roleid);
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference roles. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get args. Contact Jackie. Exiting..." << endl;
         exit(1);
      }
}

/*
vector<string>& yisigraph_t::get_sentence(int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return inpsrlgraph_m.get_sentence();
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input sentence. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         return hypsrlgraph_m.get_sentence();
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return refsrlgraph_m[refid].get_sentence();
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference sentence. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get sentence. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}
*/

vector<string> yisigraph_t::get_role_filler_units(srlnid_type roleid, int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return inpsrlgraph_m.get_role_filler_units(roleid);
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input role fillers. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         return hypsrlgraph_m.get_role_filler_units(roleid);
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return refsrlgraph_m[refid].get_role_filler_units(roleid);
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference role fillers. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get role fillers. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

double yisigraph_t::get_rolespanlength(srlnid_type roleid, int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return spanlength(inpsrlgraph_m.get_role_span(roleid));
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input role span length. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         return spanlength(hypsrlgraph_m.get_role_span(roleid));
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return spanlength(refsrlgraph_m[refid].get_role_span(roleid));
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference role span length. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get role span length. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

yisigraph_t::label_type yisigraph_t::get_rolelabel(srlnid_type roleid, int mode, int refid) {
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            return inpsrlgraph_m.get_role_label(roleid);
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input role label. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         return hypsrlgraph_m.get_role_label(roleid);
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            return refsrlgraph_m[refid].get_role_label(roleid);
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference role label. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get role label. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

vector<pair<int, yisigraph_t::alignment_type> > yisigraph_t::get_hypalignment(srlnid_type roleid) {
   vector<pair<int, yisigraph_t::alignment_type> > result;
   if (hypalignment_m.find(roleid) != hypalignment_m.end()) {
      result = hypalignment_m[roleid];
   }
   return result;
}

double yisigraph_t::get_alignsim(srlnid_type roleid, int mode, int refid) {
   double result = 0.0;
   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            if (inpalignment_m.find(roleid) != inpalignment_m.end()) {
               return inpalignment_m[roleid].second;
            } else {
               return 0.0;
            }
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input alignment sim. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         // return the maximum sim of all reference and input
         // better not to use this because it lost align label information
         if (hypalignment_m.find(roleid) != hypalignment_m.end()) {
            for (auto it = hypalignment_m[roleid].begin();
                  it != hypalignment_m[roleid].end(); it++) {
               double s = (it->second).second;
               if (s > result) {
                  result = s;
               }
            }
         }
         return result;
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            if (refalignment_m[refid].find(roleid) != refalignment_m[refid].end()) {
               return refalignment_m[refid][roleid].second;
            } else {
               return 0.0;
            }
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference alignment sim. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get align sim. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

yisigraph_t::label_type yisigraph_t::get_alignlabel(srlnid_type roleid, int mode, int refid) {
   double m = 0.0;
   label_type result;

   switch (mode) {
      case yisi::INP_MODE:
         if (inp_b) {
            if (inpalignment_m.find(roleid) != inpalignment_m.end()) {
               return hypsrlgraph_m.get_role_label(inpalignment_m[roleid].first);
            } else {
               return label_type("U");
            }
         } else {
            cerr << "ERROR: YiSi graph with no input sentence. "
                 << "Failed to get input alignment role label. Exiting..." << endl;
            exit(1);
         }
         break;
      case yisi::HYP_MODE:
         // return the role label of the max align sim of all reference and input
         // better not to use this
         if (hypalignment_m.find(roleid) != hypalignment_m.end()) {
            for (auto it = hypalignment_m[roleid].begin();
                  it != hypalignment_m[roleid].end(); it++) {
               double s = (it->second).second;
               if (s > m) {
                  int id = it->first;
                  if (id < (int)refsrlgraph_m.size()) {
                     result = refsrlgraph_m[id].get_role_label((it->second).first);
                  } else {
                     result = inpsrlgraph_m.get_role_label((it->second).first);
                  }
               }
            }
         }
         return result;
         break;
      case yisi::REF_MODE:
         if (-1 < refid && refid < (int)refsrlgraph_m.size()) {
            if (refalignment_m[refid].find(roleid) != refalignment_m[refid].end()) {
               return hypsrlgraph_m.get_role_label(refalignment_m[refid][roleid].first);
            } else {
               return label_type("U");
            }
         } else {
            cerr << "ERROR: refid (" << refid << ") out of range [0," << refsrlgraph_m.size()
                 << "]. Failed to get reference alignment role label. Exiting..." << endl;
            exit(1);
         }
         break;
      default:
         cerr << "ERROR: Unknown mode in get align label. Contact Jackie. Exiting..." << endl;
         exit(1);
   }
}

double yisigraph_t::spanlength(span_type span) {
   return span.second - span.first;
}

void yisigraph_t::print(ostream& os) {
   string h = yisi::join(hypsrlgraph_m.get_role_filler_units(hypsrlgraph_m.get_root()), " ");
   //os << h <<endl;
   for (size_t i = 0; i < refalignment_m.size(); i++) {
      string r = yisi::join(refsrlgraph_m[i].get_role_filler_units(refsrlgraph_m[i].get_root()), " ");
      //os << r <<endl;
      for (auto jt = refalignment_m[i].begin(); jt != refalignment_m[i].end(); jt++) {
         auto refnid = jt->first;
         auto hypnid = (jt->second).first;
         double sim = (jt->second).second;
         r = yisi::join(refsrlgraph_m[i].get_role_filler_units(refnid), " ");
         h = yisi::join(hypsrlgraph_m.get_role_filler_units(hypnid), " ");
         os << r << "\t" << h << "\t" << sim << endl;
      }
   }
   if (inp_b) {
      string inp = yisi::join(inpsrlgraph_m.get_role_filler_units(inpsrlgraph_m.get_root()), " ");
      os << inp << endl;
      for (auto kt = inpalignment_m.begin(); kt != inpalignment_m.end(); kt++) {
         auto inpnid = kt->first;
         auto hypnid = (kt->second).first;
         double sim = (kt->second).second;
         inp = yisi::join(inpsrlgraph_m.get_role_filler_units(inpnid), " ");
         h = yisi::join(hypsrlgraph_m.get_role_filler_units(hypnid), " ");
         os << inp << "\t" << h << "\t" << sim << endl;
      }
   }
}
