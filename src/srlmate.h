/**
 * @file srlmate.h
 * @brief SRL MATE
 *
 * @author Jackie Lo
 *
 * Class definition of:
 *    - srlmate (wrapper class for MATE srl tool)
 *
 * ***IMPORTANT***: the path for the constructor here is the config file of MATE
 * for the specific language.
 * Please edit the config file in MATE correctly for calling the right model.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef SRLMATE_H
#define SRLMATE_H

#include "srlgraph.h"
#include "srlutil.h"
#include <jni.h>

#include <string>
#include <vector> 

namespace yisi {

   class srlmate_t:public srlmodel_t{
   public:
      srlmate_t() {}
      srlmate_t(std::string path);
      ~srlmate_t();
      std::string jrun(std::string sent);
      srlgraph_t parse(std::string sent);
      virtual std::vector<srlgraph_t> parse(std::vector<std::string> sents);
   private:
      std::string noparse(std::vector<std::string> tokens);
      static JavaVM* jvm_m;
      static JNIEnv* jen_m;
      static int obj_cnt_m;
      jclass mate_class_m;
      jobject mate_object_m;
   };

} // yisi

#endif
