/**
 * @file sent.cpp
 * @brief Sentence
 *
 * @author Jackie Lo
 *
 * Class implementation for the classes:
 *    - sent_t
 * and the definitions of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2019, Her Majesty in Right of Canada /
 * Copyright 2019, Sa Majeste la Reine du Chef du Canada
 */

#include "sent.h"

#include <fstream>
#include <sstream>
#include <math.h>

using namespace yisi;
using namespace std;

sent_t::sent_t() {
   sent_type_m = "word";
}

sent_t::sent_t(string sent_type) {
   sent_type_m = sent_type;
}

sent_t::sent_t(const sent_t& rhs) {
   sent_type_m = rhs.sent_type_m;
   token_m = rhs.token_m;
   unit_m = rhs.unit_m;
   emb_m = rhs.emb_m;
   tid2uspan_m = rhs.tid2uspan_m;
   uid2tid_m = rhs.uid2tid_m;
}

void sent_t::operator=(const sent_t& rhs) {
   sent_type_m = rhs.sent_type_m;
   token_m = rhs.token_m;
   unit_m = rhs.unit_m;
   emb_m = rhs.emb_m;
   tid2uspan_m = rhs.tid2uspan_m;
   uid2tid_m = rhs.uid2tid_m;
}

string sent_t::get_type() {
   return sent_type_m;
}

vector<string> sent_t::get_tokens(span_type tspan) {
   vector<string> result;
   for (size_t i = tspan.first; i < tspan.second; i++) {
      result.push_back(token_m[i]);
   }
   /*
   cerr << "In get_tokens(" << tspan.first << "," << tspan.second << "): ";
   for (auto it = result.begin(); it != result.end(); it++) {
      cerr << *it << " ";
   }
   cerr << endl;
   */
   return result;
}

vector<string> sent_t::get_tokens() {
   // cerr << "In get_tokens(): " << endl;
   return token_m;
}

vector<string> sent_t::get_units(span_type uspan) {
   vector<string> result;
   if (sent_type_m == "word") {
      for (size_t i = uspan.first; i < uspan.second; i++) {
         result.push_back(token_m[i]);
      }
   } else {
      for (size_t i = uspan.first; i < uspan.second; i++) {
         if (i < unit_m.size()) {
            result.push_back(unit_m[i]);
         }
      }
   }
   return result;
}

vector<vector<double> > sent_t::get_embs(span_type uspan) {
   if (sent_type_m == "uemb") {
      vector<vector<double> > result;
      for (size_t i = uspan.first; i < uspan.second; i++) {
         result.push_back(emb_m[i]);
      }
      return result;
   } else {
      cerr << "ERROR: sentence type (" << sent_type_m << ") "
           << "does not provide contextual embeddings. Exiting..." << endl;
      exit(1);
   }
}

sent_t::span_type sent_t::tspan2uspan(span_type tspan) {
   if (sent_type_m == "word") {
      return tspan;
   } else {
      //cerr << tid2uspan_m.size();
      if (tspan.first < tid2uspan_m.size() && (tspan.second-1) < tid2uspan_m.size()) {
         return span_type(tid2uspan_m[tspan.first].first, tid2uspan_m[tspan.second-1].second);
      } else {
         return tspan;
      }
   }
}

sent_t::span_type sent_t::uspan2tspan(span_type uspan) {
   if (sent_type_m == "word") {
      return uspan;
   } else {
      return span_type(uid2tid_m[uspan.first], uid2tid_m[uspan.second-1]);
   }
}

void sent_t::set_tokens(vector<string> t) {
   token_m = t;
   /*
   cerr << "In set_tokens(t): ";
   for (auto it = token_m.begin(); it != token_m.end(); it++) {
      cerr << *it << " ";
   }
   cerr << endl;
   */
}

void sent_t::set_units(vector<string> u ) {
   unit_m = u;
}

void sent_t::set_embs(vector<vector<double> > e) {
   emb_m = e;
}

void sent_t::set_tid2uspan(vector<span_type> t2u) {
   tid2uspan_m = t2u;
}

void sent_t::set_uid2tid(vector<size_t> u2t) {
   uid2tid_m = u2t;
}

size_t sent_t::get_token_size() {
   return token_m.size();
}

vector<sent_t*> yisi::read_sent(string sent_type, string token_path, string unit_path, string idemb_path) {
   vector<sent_t*> result;
   vector<vector<double> > emb;
   vector<sent_t::span_type> t2u;
   vector<size_t> u2t;
   size_t currtid = (size_t)-1;

   //cerr << token_path << " ";
   auto token_strs = read_file(token_path);
   if (unit_path == "") {
      for (auto tt = token_strs.begin(); tt != token_strs.end(); tt++) {
         sent_t* sent_p = new sent_t(sent_type);
         sent_p->set_tokens(tokenize(*tt));
         //cerr << "sentlength=" << sent.get_token_size();
         result.push_back(sent_p);
         //cerr << " Done." << endl;
      }

   } else {
      // cerr << unit_path << " ";
      auto unit_strs = read_file(unit_path);
      auto tt = token_strs.begin();
      auto ut = unit_strs.begin();
      //cerr << idemb_path << " ";
      ifstream fin(idemb_path.c_str());
      if (!fin) {
         cerr << "ERROR: Failed to open idemb file (" << idemb_path << "). Exiting..." << endl;
         exit(1);
      }
      while (!fin.eof()) {
         string line;
         getline(fin, line);
         if (line.empty()) {
            sent_t* s = new sent_t(sent_type);
            auto tokens = tokenize(*tt);
            s->set_tokens(tokens);
            //cerr << "#token=" << tokens.size();
            auto units = tokenize(*ut);
            s->set_units(units);
            //cerr << " #unit=" << units.size();
            if (sent_type == "uemb") {
               s->set_embs(emb);
            }
            //cerr << " #emb=" << emb.size() << " #dim=" << emb[0].size();
            s->set_tid2uspan(t2u);
            //cerr << " #tid=" << t2u.size();
            s->set_uid2tid(u2t);
            //cerr << " #uid=" << u2t.size() << " ";

            result.push_back(s);
            tt++;
            ut++;
            emb.clear();
            t2u.clear();
            u2t.clear();
            currtid = (size_t)-1;
         } else {
            istringstream iss(line);
            size_t uid;
            size_t tid;
            iss >> uid >> tid;
            u2t.push_back(tid);
            if (tid != currtid) {
               t2u.push_back(sent_t::span_type(uid,uid+1));
               currtid=tid;
            } else {
               t2u.back().second=uid+1;
            }
            if (sent_type == "uemb") {
               vector<double> e;
               double len = 0.0;
               double v;
               while (!iss.eof()) {
                  iss >> v;
                  e.push_back(v);
                  len += v*v;
               }
               len = sqrt(len);
               for (size_t i = 0; i < e.size(); i++) {
                  e[i] /= len;
               }
               emb.push_back(e);
            }
         }
         fin.peek();
      }
      fin.close();
   }
   return result;
}
