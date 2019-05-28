/**
 * @file lexsim.h
 * @brief Lexical similarity
 *
 * @author Jackie Lo
 *
 * Class definition of lexical similarity classes:
 *    - lexsim_t (wrapper class)
 *    - lexsimmodel_t (abstract base class of different lex sim models)
 *    - lexsimexact_t (simiple exact match lex sim model)
 *    - lexsimibm_t (ibm1 lex sim model: crosslingual) ***deprecated: performance not as good as biw2v*** 
 *    - lexsimw2v_t (w2v lex sim model)
 *    - lexsimemapw2v_t (emapw2v lex sim model: crosslingual)
 *    - lexsimbiw2v_t (biw2v lex sim model: crosslingual)
 *    - lexsimdcv_t (discrete context vector lex sim model)
 * and the declaration of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef LEXSIM_H
#define LEXSIM_H

#include "util.h"

#include <utility>
#include <string>
#include <vector> 
#include <map>
#include <iostream>

namespace yisi {

   static std::map<std::string, std::map<std::string, double> > mlscache_m;
   static std::map<std::string, std::map<std::string, double> > xlscache_m;

   class lexsimmodel_t {
   public:
      lexsimmodel_t() { eps_m=0.00001; }
      virtual ~lexsimmodel_t() {}
      virtual std::vector<double>& get_wv(std::string word, int mode) {
         std::cerr << "ERROR: lexsim model is not a word vector model. Exiting..." << std::endl;
         exit(1);
      }
      virtual double get_sim(std::string ref, std::string hyp, int mode)=0;
      virtual double get_sim(std::vector<double>& ref, std::vector<double>& hyp) {
         std::cerr << "ERROR: lexsim model is not a word vector model" << std::endl;
         return 0.0;
      }
      virtual void write_txtw2v(std::string path) {
         std::cerr << "ERROR: lexsim model is not a word vector model" << std::endl;
         exit(1);
      }
   protected:
      double eps_m;
   }; // class lexsimmodel_t
  
   class lexsimexact_t:public lexsimmodel_t {
   public:
      lexsimexact_t() {
         std::cerr << "Constructing exact lexsim model" << std::endl;
      }
      virtual ~lexsimexact_t() {}
      virtual double get_sim(std::string ref, std::string hyp, int mode);
   private:
   }; // class lexsimexact_t

   class lexsimlcs_t:public lexsimmodel_t {
   public:
      lexsimlcs_t() {
         std::cerr << "Constructing lcs lexsim model" << std::endl;
      };
      virtual ~lexsimlcs_t() {}
      virtual double get_sim(std::string ref, std::string hyp, int mode);
   private:
   }; // class lexsimlcs_t

   class lexsimw2v_t:public lexsimmodel_t {
   public:
      lexsimw2v_t() {
         dimension_m = 0;
         func_m = "";
      }
      lexsimw2v_t(std::string outw2v_path);
      virtual ~lexsimw2v_t() {}
      std::vector<double>& get_wv(std::string word, int mode);
      virtual double get_sim(std::string ref, std::string hyp, int mode);
      virtual double get_sim(std::vector<double>& ref, std::vector<double>& hyp);
      void write_txtw2v(std::string path);
   protected:
      std::map<std::string, std::vector<double> > outembeddings_m;
      std::string func_m;
      int dimension_m;
   }; // class lexsimw2v_t

   class lexsimemb_t:public lexsimmodel_t {
   public:
      lexsimemb_t() {
         func_m = "cosine";
      }
      virtual ~lexsimemb_t() {}
      virtual double get_sim(std::string ref, std::string hyp, int mode);
      virtual double get_sim(std::vector<double>& ref, std::vector<double>& hyp);
   protected:
      std::string func_m;
   }; // class lexsimw2v_t

   class lexsimemapw2v_t:public lexsimw2v_t {
   public:
      lexsimemapw2v_t() {}
      lexsimemapw2v_t(std::string emap_path, std::string outw2v_func);
      virtual ~lexsimemapw2v_t() {}
      std::vector<double>& get_wv(std::string word, int mode);
      virtual double get_sim(std::string s1, std::string hyp, int mode);
      virtual double get_sim(std::vector<double>& s1, std::vector<double>& hyp);
   private:
      std::map<std::string, std::string> emap_m;
   }; // class lexsimemapw2v_t

//   class lexsimibm_t:public lexsimmodel_t {
//   public:
//      lexsimibm_t() {}
//      lexsimibm_t(std::string inp2hyp_path, std::string hyp2inp_path);
//      virtual ~lexsimibm_t() {}
//      virtual double get_sim(std::string inp, std::string hyp);
//   private:
//      std::map<std::pair<std::string, std::string>, double> pfge_m;
//      std::map<std::pair<std::string, std::string>, double> pegf_m;
//   }; // class lexsimibm_t

   class lexsimbiw2v_t:public lexsimw2v_t {
   public:
      lexsimbiw2v_t() {}
      lexsimbiw2v_t(std::string inpw2v_path, std::string outw2v_path);
      virtual ~lexsimbiw2v_t() {};
      std::vector<double>& get_wv(std::string word, int mode);
      virtual double get_sim(std::string s1, std::string hyp, int mode);
      virtual double get_sim(std::vector<double>& s1, std::vector<double>& hyp);
   private:
      std::map<std::string, std::vector<double> > inpembeddings_m;
   }; // class lexsimbiw2v_t

   class lexsim_t {
   public:
      lexsim_t();
      lexsim_t(std::string name, std::string out_path="", std::string inp_path="");
      lexsim_t(lexsim_t& rhs);
      ~lexsim_t();
      double get_sim(std::string s1, std::string hyp, int mode);
      double get_sim(std::vector<double>& s1, std::vector<double>& hyp);
      std::vector<double>& get_wv(std::string word, int mode);
      void write_txtw2v(std::string path) { lexsim_p->write_txtw2v(path); }
   private:
      lexsimmodel_t* lexsim_p;
      std::string lexsim_name_m;
      std::string outlexsim_path_m;
      std::string inplexsim_path_m;
   }; // class lexsim_t

   void read_binw2v(std::string path, std::map<std::string, std::vector<double> >& model, int& dimension);
   void read_txtw2v(std::string path, std::map<std::string, std::vector<double> >& model, int& dimension);
   std::vector<double>& get_wv(std::map<std::string, std::vector<double> >& model, std::string word);
   double get_sim(std::vector<double>& v1, std::vector<double>& v2, std::string func);
   bool sort_helper(std::pair<std::string, double> i, std::pair<std::string, double> j);
   double simfunc(std::string funcname, std::vector<double>& ref, std::vector<double>& hyp);
   double cosine(std::vector<double>& ref, std::vector<double>& hyp, int mode);
   double jaccard(std::vector<double>& ref, std::vector<double>& hyp, int mode);

} // yisi

#endif
