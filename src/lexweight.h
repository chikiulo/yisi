/**
 * @file lexweight.h
 * @brief Lexical weight
 *
 * @author Jackie Lo
 *
 * Class definition of lexical weight classes:
 *    - lexweight_t (wrapper class)
 *    - lexweightmodel_t (abstract base class of different lex weight models)
 *    - lexweightuniform_t (simple uniform lexical weight)
 *    - lexweightfile_t (read lexical weight model from file)
 *    - lexweightlearn_t (estimate lexical weight from either a range of ranges of tokens or a file)
 * and the declaration of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef LEXWEIGHT_H
#define LEXWEIGHT_H

#include <string>
#include <vector> 
#include <map>
#include <iostream>

namespace yisi {

   class lexweightmodel_t {
   public:
      lexweightmodel_t():N(0.0) {}
      lexweightmodel_t(std::vector<std::vector<std::string> > tokens) {
         std::cerr << "ERROR: lexweight model is not learning from the reference" << std::endl;
      }
      lexweightmodel_t(std::string path) {
         std::cerr << "ERROR: lexweight model does not require a file path" << std::endl;
      }
      virtual ~lexweightmodel_t() {}

      virtual double get_weight(std::string lex);
      void write(std::ostream& os);
      void read(std::string path);
   protected:
      std::map<std::string, double> lexweight_m;
      double N;
   }; // class lexweightmodel_t

   class lexweightuniform_t:public lexweightmodel_t {
   public:
      lexweightuniform_t() {}
      virtual ~lexweightuniform_t() {}
      virtual double get_weight(std::string lex) { return 1.0; }
   private:
   }; // class lexweightuniform_t

   class lexweightfile_t:public lexweightmodel_t {
   public:
      lexweightfile_t() {}
      lexweightfile_t(std::string path);
      virtual ~lexweightfile_t() {}
   private:
   }; // class lexweightfile_t

   class lexweightlearn_t:public lexweightmodel_t {
   public:
      lexweightlearn_t() {}
      lexweightlearn_t(std::string path);
      lexweightlearn_t(std::vector<std::vector<std::string> > tokens);
      lexweightlearn_t(lexweightlearn_t& rhs);
      virtual ~lexweightlearn_t() {}
      void learn(std::vector<std::vector<std::string> > tokens);
   private:
   }; // class lexweightlearn_t

   class lexweight_t {
   public:
      lexweight_t();
      lexweight_t(std::string name, std::string path="");
      lexweight_t(std::vector<std::vector<std::string> > tokens);
      lexweight_t(lexweight_t& rhs);
      ~lexweight_t();
      double operator()(std::string lex);
      void write(std::ostream& os);
   private:
      lexweightmodel_t* lexweight_p;
      std::string lexweight_name_m;
      std::string lexweight_path_m;
   }; // class lexweight_t

} // yisi

#endif
