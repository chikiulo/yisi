/**
 * @file srlgraph.h
 * @brief SRL graph
 *
 * @author Jackie Lo
 *
 * Class definition of one class:
 *    - srlgraph_t
 * and the declaration of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef SRLGRAPH_H
#define SRLGRAPH_H

#include "graph.h"
#include "sent.h"
#include <utility>
#include <string>
#include <vector> 
#include <map>
#include <iostream>

namespace yisi {

   class srlgraph_t {
   public:
      typedef sent_t::span_type span_type;
      typedef std::string label_type;
      typedef graph_t<span_type, label_type>::node_type srlnode_type;
      typedef graph_t<span_type, label_type>::edge_type srledge_type;
      typedef graph_t<span_type, label_type>::nid_type srlnid_type;
      typedef graph_t<span_type, label_type>::eid_type srleid_type;


      srlgraph_t();
      srlgraph_t(sent_t* sent);
      srlgraph_t(const srlgraph_t& rhs);
      void operator=(const srlgraph_t& rhs);

      srlnid_type new_root();
      srlnid_type new_root(sent_t* sent);
      srlnid_type new_pred();
      srlnid_type new_pred(span_type& span, label_type& label);
      srlnid_type new_arg(srlnid_type predid);
      srlnid_type new_arg(srlnid_type predid, span_type& span, label_type& label);

      srlnid_type get_root();
      std::vector<srlnid_type> get_preds();
      std::vector<srlnid_type> get_args(srlnid_type predid);

      srlnid_type get_pred(srlnid_type argid);

      std::vector<std::string> get_sentence();
      std::vector<std::string> get_role_filler_units(srlnid_type roleid);
      std::vector<std::vector<double> > get_role_filler_embs(srlnid_type roleid);

      label_type get_role_label(srlnid_type roleid);
      span_type get_role_span(srlnid_type roleid);
      std::string get_sent_type(){return sent_p->get_type();};
      size_t get_sent_length();

      void set_tokens(std::vector<std::string>& tokens);
      void set_sent(sent_t* sent);
      void set_role_span(srlnid_type predid, span_type& span);
      void set_role_label(srlnid_type predid, label_type& label);

      std::ostream& operator<<(std::ostream& os);
      void print(std::ostream& os, int i);

      void delete_sent();

   private:
      graph_t<span_type, label_type> srl_m;
      sent_t* sent_p;
      // std::vector<std::string> tokens_m;
      srlnid_type root_m;
      std::map<srlnid_type, srlnid_type> predof_m;
   }; // class srlgraph_t
   
   std::ostream& operator<<(std::ostream& os, srlgraph_t& srl);

} // yisi

#endif
