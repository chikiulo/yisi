/**
 * @file sent.h
 * @brief Sentence
 *
 * @author Jackie Lo
 *
 * Class definition of sentence classes:
 *    - sent_t
 * and the declaration of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#ifndef SENT_H
#define SENT_H

#include "util.h"

#include <utility>
#include <string>
#include <vector> 
#include <map>
#include <iostream>

namespace yisi {

   class sent_t {
   public:
      typedef std::pair<size_t, size_t> span_type;
      sent_t();
      sent_t(std::string sent_type);
      sent_t(const sent_t& rhs);
      void operator=(const sent_t& rhs);
      ~sent_t() {};
      std::string get_type();
      std::vector<std::string> get_tokens(span_type tspan);
      std::vector<std::string> get_tokens();
      std::vector<std::string> get_units(span_type uspan);
      std::vector<std::vector<double> > get_embs(span_type uspan);
      void set_tokens(std::vector<std::string> t);
      void set_units(std::vector<std::string> u);
      void set_embs(std::vector<std::vector<double> > e);
      void set_tid2uspan(std::vector<span_type> t2u);
      void set_uid2tid(std::vector<size_t> u2t);
      span_type tspan2uspan(span_type tspan);
      span_type uspan2tspan(span_type uspan);
      size_t get_token_size();
   private:
      std::string sent_type_m;
      std::vector<std::string> token_m;
      std::vector<std::string> unit_m;
      std::vector<std::vector<double> > emb_m;
      std::vector<span_type> tid2uspan_m;
      std::vector<size_t> uid2tid_m;
   }; // class sent_t

   std::vector<sent_t*> read_sent(std::string sent_type, std::string token_path, std::string unit_path="", std::string idemb_path="");

} // yisi

#endif
