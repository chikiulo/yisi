/**
 * @file graph.h
 * @brief Generic graph API for general purpose.
 *
 * @author Jackie Lo
 *
 * Class definition of three general purpose graph classes:
 *    - node_t
 *    - edge_t
 *    - graph_t
 *
 * ATTENTION: This graph class is not the same as the yisigraph class!
 *    Please see yisigraph.h if you are looking for the definition of yisigraph.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <vector> 

namespace yisi {

   template <class data_T>
   class node_t {
   public:
      typedef data_T data_type;

      node_t() {}
      node_t(data_type& data):data_m(data) {}
      node_t(const node_t& rhs) {
         data_m = rhs.data_m;
      }

      data_type get_data() { return data_m; }

      void set_data(data_type& data) { data_m = data; }

   private:
      data_type data_m;
   }; // class node_t

   template <class label_T>
   class edge_t {
   public:
      typedef label_T label_type;
      typedef std::size_t nid_type;

      edge_t() {};
      edge_t(nid_type tail, nid_type head, label_type& label)
         :tail_m(tail), head_m(head), label_m(label) {}
      edge_t(const edge_t& rhs) {
         tail_m = rhs.tail_m;
         head_m = rhs.head_m;
         label_m = rhs.label_m;
      }

      nid_type get_head() { return head_m; }
      nid_type get_tail() { return tail_m; }
      label_type get_label() { return label_m; }

      void set_head(nid_type head) { head_m = head; }
      void set_tail(nid_type tail) { tail_m = tail; }
      void set_label(label_T& label) { label_m = label; }

   private:
      nid_type tail_m;
      nid_type head_m;
      label_type label_m;
   }; // class edge_t


   template <class data_T, class label_T>
   class graph_t {
   public:
      typedef data_T data_type;
      typedef label_T label_type;
      typedef node_t<data_type> node_type;
      typedef edge_t<label_type> edge_type;
      typedef typename edge_type::nid_type nid_type;
      typedef std::size_t eid_type;

      /// Default constructor
      graph_t() {};
      graph_t(const graph_t& rhs) {
         nodes_m = rhs.nodes_m;
         edges_m = rhs.edges_m;
         incoming_m = rhs.incoming_m;
         outgoing_m = rhs.outgoing_m;
      }

      void operator=(const graph_t& rhs) {
         nodes_m = rhs.nodes_m;
         edges_m = rhs.edges_m;
         incoming_m = rhs.incoming_m;
         outgoing_m = rhs.outgoing_m;
      }

      nid_type new_node() {
         nodes_m.push_back(node_t<data_T>());
         incoming_m.push_back(std::vector<eid_type>());
         outgoing_m.push_back(std::vector<eid_type>());
         return nodes_m.size() - 1;
      }

      nid_type new_node(data_type& data) {
         nodes_m.push_back(node_t<data_T>(data));
         incoming_m.push_back(std::vector<eid_type>());
         outgoing_m.push_back(std::vector<eid_type>());
         return nodes_m.size() - 1;
      }

      eid_type new_edge(nid_type tail, nid_type head, label_type& label) {
         edges_m.push_back(edge_t<label_T>(tail, head, label));
         eid_type eid = edges_m.size() - 1;
         incoming_m.at(head).push_back(eid);
         outgoing_m.at(tail).push_back(eid);
         return eid;
      }

      nid_type get_edge_head(eid_type eid) { return edges_m.at(eid).get_head(); }
      nid_type get_edge_tail(eid_type eid) { return edges_m.at(eid).get_tail(); }
      label_type get_edge_label(eid_type eid) { return edges_m.at(eid).get_label(); }

      data_type get_node_data(nid_type nid) { return nodes_m.at(nid).get_data(); }

      std::size_t get_node_count() { return nodes_m.size(); }

      const std::vector<eid_type>& get_incoming_edges(nid_type nid){ return incoming_m.at(nid); }
      const std::vector<eid_type>& get_outgoing_edges(nid_type nid){ return outgoing_m.at(nid); }

      // We don't support changes to graph topology once it is created at the moment
      // i.e. set_edge_head, set_edge_tail, delete_edge, delete_node are not suppported

      void set_edge_label(eid_type eid, label_type label) { edges_m.at(eid).set_label(label); }
      void set_node_data(nid_type nid, data_type data) { nodes_m.at(nid).set_data(data); }

   private:
      std::vector<node_type> nodes_m;
      std::vector<edge_type> edges_m;
      std::vector<std::vector<eid_type> > incoming_m;
      std::vector<std::vector<eid_type> > outgoing_m;
   }; // class graph_t

} // yisi

#endif
