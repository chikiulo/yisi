/**
 * @file lexweight.cpp
 * @brief Lexical weight
 *
 * @author Jackie Lo
 *
 * Class implementation for the classes:
 *    - lexweightmodel_t (abstract base class of different lex weight models)
 *    - lexweightfile_t (read lexical weight model from file)
 *    - lexweightlearn_t (estimate lexical weight from either a range of ranges of tokens or a file)
 *    - lexweight_t (wrapper class)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "lexweight.h"
#include "util.h"

#include <fstream>
#include <math.h>
#include <set>

using namespace yisi;
using namespace std;

double lexweightmodel_t::get_weight(string lex) {
  double c;
  auto it = lexweight_m.find(lex);
  if (it != lexweight_m.end()) {
    c = it->second;
  } else {
    string l = lowercase(lex);
    it=lexweight_m.find(l);
    if (it != lexweight_m.end()) {
      c = it->second;
    } else {
      c=0.0;
    }
  }
  return log2(1 + ((N + 1.0) / (c + 1.0)));
}

void lexweightmodel_t::write(std::ostream& os) {
  os<<N<<endl;
  for (auto it = lexweight_m.begin(); it != lexweight_m.end(); it++) {
    os << it->second << " " << it->first << endl;
  }
}

void lexweightmodel_t::read(string path) {
  cerr << "Reading lex weight file from " << path << " ... ";
  ifstream WT(path.c_str());
  if (!WT) {
    cerr << "ERROR: Failed to open weight table file. Exiting..." << endl;
    exit(1);
  }
  double n;
  WT>>n;
  N+=n;
  while (!WT.eof()) {
    string lex;
    double c;
    WT>>c>>lex;
    auto it = lexweight_m.find(lex);
    if (it !=lexweight_m.end()){
      it->second+=c;
    }else{
      lexweight_m[lex]=c;
    }
  }
  WT.close();
  cerr << "Done." << endl;
}

lexweightfile_t::lexweightfile_t(string path) {
  cerr << "Reading lex weight file from " << path << " ... ";
  ifstream WT(path.c_str());
  if (!WT) {
    cerr << "ERROR: Failed to open weight table file. Exiting..." << endl;
    exit(1);
  }
  WT>>N;
  while (!WT.eof()) {
    string lex;
    double weight;
    WT >> weight >> lex;
    lexweight_m[lex] = weight;
  }
  
  WT.close();
  cerr << "Done." << endl;
}

lexweightlearn_t::lexweightlearn_t(vector<vector<string> > tokens) {
   learn(tokens);
}

lexweightlearn_t::lexweightlearn_t(lexweightlearn_t& rhs) {
   lexweight_m = rhs.lexweight_m;
}

lexweightlearn_t::lexweightlearn_t(string path) {
   vector<vector<string> > tokens;
   auto paths = tokenize(path,':');
   for (auto it=paths.begin(); it!=paths.end(); it++){
     cerr << "Learning lex weight from " << *it << " ... ";
     vector<string> rs = read_file(*it);
     for (auto jt=rs.begin(); jt!=rs.end(); jt++){
       auto ts = tokenize(*jt);
       tokens.push_back(ts);
     }
   }
   learn(tokens);
   cerr << "Done." << endl;
}

void lexweightlearn_t::learn(vector<vector<string> > tokens) {
   N += tokens.size();

   for (auto it = tokens.begin(); it != tokens.end(); it++) {
      set<string> sent;
      sent.insert(it->begin(), it->end());

      for (auto jt = sent.begin(); jt != sent.end(); jt++) {
	auto kt=lexweight_m.find(*jt);
         if (kt != lexweight_m.end()) {
            kt->second += 1.0;
         } else {
            lexweight_m[*jt] = 1.0;
         }
      }
   }
}

lexweight_t::lexweight_t() {
   lexweight_p = new lexweightuniform_t();
}

lexweight_t::lexweight_t(string name, string path) {
   if (name == "uniform") {
      lexweight_p = new lexweightuniform_t();
   } else if (name == "file") {
      lexweight_p = new lexweightfile_t(path);
   } else if (name == "learn") {
      lexweight_p = new lexweightlearn_t(path);
   } else {
      cerr << "ERROR: Unknown lexweight model type " << name << endl;
   }
   lexweight_name_m = name;
   lexweight_path_m = path;
}

lexweight_t::lexweight_t(vector<vector<string> > tokens) {
   lexweight_p = new lexweightlearn_t(tokens);
   lexweight_name_m = "tokens";
}

lexweight_t::lexweight_t(lexweight_t& rhs) {
   if (lexweight_name_m == "uniform") {
      lexweight_p = new lexweightuniform_t();
   } else if (lexweight_name_m == "file") {
      lexweight_p = new lexweightfile_t(lexweight_path_m);
   } else if (lexweight_name_m == "learn") {
      lexweight_p = new lexweightlearn_t(lexweight_path_m);
   } else if (lexweight_name_m == "tokens") {
      lexweight_p = new lexweightlearn_t((lexweightlearn_t&)rhs);
   }
   lexweight_name_m = rhs.lexweight_name_m;
   lexweight_path_m = rhs.lexweight_path_m;
}


lexweight_t::~lexweight_t() {
   if (lexweight_p != NULL) {
      delete lexweight_p;
   }
}

double lexweight_t::operator()(string lex) {
   double w = lexweight_p->get_weight(lex);
   //cerr << "Lexweight of " << lex << "=" << w << endl;
   return w;
}

void lexweight_t::write(ostream& os) {
   lexweight_p->write(os);
}
