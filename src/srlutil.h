/**
 * @file srlutil.h
 * @brief SRL utility functions
 *
 * @author Jackie Lo
 *
 * Class definition of srl utility classes:
 *    - srlmodel_t (abstract base class of different semantic role labelers)
 *    - srlread_t
 *    - srltok_t
 * and the declaration of some utility functions working on srl.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef SRLUTIL_H
#define SRLUTIL_H

#include "srlgraph.h"

#include <set>
#include <string>
#include <vector> 
#include <map>

namespace yisi {

   std::vector<srlgraph_t> read_srl(std::vector<sent_t*> sents, std::string parsefile);
   srlgraph_t read_conll09(std::string parse, sent_t* sent);
   srlgraph_t read_conll09(std::string parse);
   void populate_label(std::vector<std::string>& labels, std::map<int, std::set<int> > child, int i);
   std::vector<srlgraph_t> read_conll09batch(std::string filename);
   std::vector<srlgraph_t> read_conll09batch(std::string filename, std::vector<sent_t*> sents);
   class srlmodel_t {
   public:
      srlmodel_t() {}
      virtual ~srlmodel_t() {}
      virtual srlgraph_t parse(sent_t* sent) {
         std::cerr << "ERROR: Semantic role labeler type does not support "
                   << "individual sentence parsing. Exiting..." << std::endl;
         exit(1);
      }
      virtual std::vector<srlgraph_t> parse(std::vector<sent_t*> sent)=0;
   }; // srlmodel_t

   class srlread_t:public srlmodel_t {
   public:
      srlread_t() {}
      srlread_t(std::string parsefile);
      virtual std::vector<srlgraph_t> parse(std::vector<sent_t*> sents);
   private:
      std::string parsefile_m;
   }; // class srlread_t

   class srltok_t:public srlmodel_t {
   public:
      srltok_t() {}
      virtual srlgraph_t parse(sent_t* sent);
      virtual std::vector<srlgraph_t> parse(std::vector<sent_t*> sents);
   private:
   }; //class srltok_t

} // yisi

#endif
