/**
 * @file srl.h
 * @brief Semantic role labelers (SRL)
 *
 * @author Jackie Lo
 *
 * Class definition of the class:
 *    - srl_t (wrapper class)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef SRL_H
#define SRL_H

#include "srlgraph.h"
#include "srlutil.h"
#include <string>
#include <vector> 
#include <iostream>

namespace yisi {

   class srl_t {
   public:
      srl_t();
      srl_t(const std::string name, const std::string path="");
      ~srl_t();
      srlgraph_t parse(sent_t* sent);
      std::vector<srlgraph_t> parse(std::vector<sent_t*> sents);
   private:
      srlmodel_t* srl_p;
   }; // class srl_t

} // yisi

#endif
