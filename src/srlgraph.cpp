/**
 * @file srlgraph.cpp
 * @brief SRL graph
 *
 * @author Jackie Lo
 *
 * Class implementation of one class:
 *    - srlgraph_t
 * and the definitions of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "srlgraph.h"
#include "util.h"

#include <fstream>
#include <sstream>

using namespace yisi;
using namespace std;

srlgraph_t::srlgraph_t() {
}

srlgraph_t::srlgraph_t(vector<string>& tokens) {
   span_type r(0, tokens.size());
   root_m = srl_m.new_node(r);
   tokens_m = tokens;
}

srlgraph_t::srlgraph_t(const srlgraph_t& rhs) {
   srl_m = rhs.srl_m;
   tokens_m = rhs.tokens_m;
   root_m = rhs.root_m;
   predof_m = predof_m;
}

void srlgraph_t::operator=(const srlgraph_t& rhs) {
   srl_m = rhs.srl_m;
   tokens_m = rhs.tokens_m;
   root_m = rhs.root_m;
   predof_m = predof_m;
}

srlgraph_t::srlnid_type srlgraph_t::new_root() {
   span_type span(0, 0);
   root_m = srl_m.new_node(span);
   return root_m;
}

srlgraph_t::srlnid_type srlgraph_t::new_root(vector<string>& tokens) {
   span_type span(0, tokens.size());
   root_m = srl_m.new_node(span);
   tokens_m = tokens;
   return root_m;
}

srlgraph_t::srlnid_type srlgraph_t::new_pred() {
   span_type span(0, 0);
   string label = "";
   srlnid_type predid = srl_m.new_node(span);
   srl_m.new_edge(predid, root_m, label);
   return predid;
}

srlgraph_t::srlnid_type srlgraph_t::new_pred(span_type& span, label_type& label) {
   srlnid_type predid = srl_m.new_node(span);
   srl_m.new_edge(predid, root_m, label);
   return predid;
}

srlgraph_t::srlnid_type srlgraph_t::new_arg(srlnid_type predid) {
   span_type span(0, 0);
   string label = "";
   srlnid_type argid = srl_m.new_node(span);
   srl_m.new_edge(argid, predid, label);
   predof_m[argid] = predid;
   return argid;
}


srlgraph_t::srlnid_type srlgraph_t::new_arg(srlnid_type predid, span_type& span, label_type& label) {
   srlnid_type argid = srl_m.new_node(span);
   srl_m.new_edge(argid, predid, label);
   predof_m[argid] = predid;
   return argid;
}

srlgraph_t::srlnid_type srlgraph_t::get_root() {
   return root_m;
}

vector<srlgraph_t::srlnid_type> srlgraph_t::get_preds() {
   return get_args(root_m);
}

vector<srlgraph_t::srlnid_type> srlgraph_t::get_args(srlnid_type predid) {
   vector<srlnid_type> args;
   vector<srleid_type> arg_edges = srl_m.get_incoming_edges(predid);
   for (vector<srleid_type>::iterator eit = arg_edges.begin(); eit != arg_edges.end(); eit++) {
      args.push_back(srl_m.get_edge_tail(*eit));
   }
   return args;
}

srlgraph_t::srlnid_type srlgraph_t::get_pred(srlnid_type argid) {
   return predof_m[argid];
}

vector<string>& srlgraph_t::get_sentence() {
   return tokens_m;
}

vector<string> srlgraph_t::get_role_fillers(srlnid_type roleid) {
   vector<string> fillers;
   span_type span = srl_m.get_node_data(roleid);

   size_t span_begin = span.first;
   size_t span_end = span.second;

   for (size_t i = span_begin; i < span_end; i++) {
      fillers.push_back(tokens_m.at(i));
   }

   return fillers;
}

srlgraph_t::label_type srlgraph_t::get_role_label(srlnid_type roleid) {
   return srl_m.get_edge_label(srl_m.get_outgoing_edges(roleid).at(0));
}

srlgraph_t::span_type srlgraph_t::get_role_span(srlnid_type roleid) {
   return srl_m.get_node_data(roleid);
}

void srlgraph_t::set_tokens(vector<string>& tokens) {
   span_type r(0, tokens.size());
   srl_m.set_node_data(root_m, r);
   tokens_m = tokens;
}

void srlgraph_t::set_role_span(srlnid_type roleid, span_type& span) {
   srl_m.set_node_data(roleid, span);
}

void srlgraph_t::set_role_label(srlnid_type roleid, label_type& label) {
   srl_m.set_edge_label(srl_m.get_outgoing_edges(roleid).at(0), label);
} 



ostream& srlgraph_t::operator<<(ostream& os) {
   vector<srlnid_type> preds = get_preds();
   if (preds.size() > 0) {
      for (vector<srlnid_type>::iterator it = preds.begin(); it != preds.end();
         it++) {
         vector<string> frame_tokens = tokens_m;
         span_type pred_span = get_role_span(*it);
         if (pred_span.first != pred_span.second) {
            frame_tokens[pred_span.first] = "[" + get_role_label(*it) + " "
                                            + frame_tokens[pred_span.first];
            frame_tokens[pred_span.second - 1] = frame_tokens[pred_span.second - 1] + "]";
            vector<srlnid_type> args = get_args(*it);
            for (vector<srlnid_type>::iterator jt = args.begin(); jt != args.end(); jt++) {
               span_type arg_span = get_role_span(*jt);
               frame_tokens[arg_span.first] = "[" + get_role_label(*jt) + " "
                                              + frame_tokens[arg_span.first];
               frame_tokens[arg_span.second - 1] = frame_tokens[arg_span.second - 1] + "]";
            }
            for (vector<string>::iterator jt = frame_tokens.begin(); jt != frame_tokens.end(); jt++) {
               os << *jt << " ";
            }
            os << endl;
         }
      }
   } else {
      for (vector<string>::iterator it = tokens_m.begin(); it != tokens_m.end(); it++) {
         os << *it << " ";
      }
      os << endl;
   }
   return os;
}

void srlgraph_t::print(ostream& os, int i) {
   vector<srlnid_type> preds = get_preds();
   if (preds.size() > 0) {
      for (vector<srlnid_type>::iterator it = preds.begin(); it != preds.end();
         it++) {
         vector<string> frame_tokens = tokens_m;
         span_type pred_span = get_role_span(*it);
         if (pred_span.first != pred_span.second) {
            frame_tokens[pred_span.first] = "[" + get_role_label(*it) + " "
                                            + frame_tokens[pred_span.first];
            frame_tokens[pred_span.second - 1] = frame_tokens[pred_span.second - 1] + "]";
            vector<srlnid_type> args = get_args(*it);
            for (vector<srlnid_type>::iterator jt = args.begin(); jt != args.end(); jt++) {
               span_type arg_span = get_role_span(*jt);
               frame_tokens[arg_span.first] = "[" + get_role_label(*jt) + " "
                                              + frame_tokens[arg_span.first];
               frame_tokens[arg_span.second - 1] = frame_tokens[arg_span.second - 1] + "]";
            }
            os << i << ": ";
            for (vector<string>::iterator jt = frame_tokens.begin(); jt != frame_tokens.end(); jt++) {
               os << *jt << " ";
            }
            os << endl;
         }
      }
   }
}

ostream& yisi::operator<<(ostream& os, srlgraph_t& srl) {
   return srl << os;
}

