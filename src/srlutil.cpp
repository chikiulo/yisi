/**
 * @file srlutil.cpp
 * @brief SRL utility functions
 *
 * @author Jackie Lo
 *
 * The definitions of some utility functions working on semantic role labeling.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "srlutil.h"
#include "util.h"

#include <map>
#include <fstream>
#include <sstream>

using namespace yisi;
using namespace std;

vector<srlgraph_t> yisi::read_srl(vector<string> sents, string parsefile) {
   // read srl in ASSERT format
   vector<srlgraph_t> result;
   typedef srlgraph_t::span_type span_type;
   typedef srlgraph_t::srlnid_type srlnid_type;

   for (vector<string>::iterator it = sents.begin(); it != sents.end(); it++) {
      vector<string> tokens = tokenize(*it);
      srlgraph_t s(tokens);
      result.push_back(s);
   }

   if (parsefile != "") {
      ifstream ifs(parsefile.c_str(), ifstream::in);
      while (!ifs.eof()) {
         string line;
         getline(ifs, line);
         if (line == "") {
            break;
         }

         istringstream iss(line);
         int id;
         iss >> id;
         string colon;
         iss >> colon;
         vector<string> tmptok;
         size_t role_begin = 0;
         string role_label;
         bool pred_span = false;
         srlnid_type predid = result.at(id).new_pred();
         size_t curr_tok_id = 0;

         while (!iss.eof()) {
            string s;
            iss >> s;
            char first = s.at(0);
            char last = s.at(s.length() - 1);

            if (first == '[') {
               string l = s.substr(1);
               if (l == "TARGET") {
                  pred_span = true;
               }
               role_begin = curr_tok_id;
               role_label = l;
            } else if (first == ']') {
               span_type span = span_type(role_begin, curr_tok_id);
               if (pred_span) {
                  result.at(id).set_role_span(predid, span);
                  result.at(id).set_role_label(predid, role_label);
                  pred_span = false;
               } else {
                  result.at(id).new_arg(predid, span, role_label);
               }
            } else if (last == ']') {
               string t = s.substr(0, s.size() - 1);
               tmptok.push_back(t);
               span_type span = span_type(role_begin, curr_tok_id + 1);
               if (pred_span) {
                  result.at(id).set_role_span(predid, span);
                  result.at(id).set_role_label(predid, role_label);
                  pred_span = false;
               } else {
                  result.at(id).new_arg(predid, span, role_label);
               }
               curr_tok_id++;
            } else {
               tmptok.push_back(s);
               curr_tok_id++;
            }
         } // while (!iss.eof())
         if ((int)tmptok.size() > 0) {
            result.at(id).set_tokens(tmptok);
         }
      } // while (!ifs.eof())
      ifs.close();
   }

   return result;
}  // read_srl

srlgraph_t yisi::read_conll09(string parse) {
   if (parse.empty()) {
      auto tokens = tokenize(parse);
      srlgraph_t re(tokens);
      return re;
   }

   srlgraph_t result;
   result.new_root();
   srlgraph_t::label_type plabel = "V";
   vector<string> tokens;
   vector<int> preds;
   map<int, srlgraph_t::srlnid_type> pids;
   vector<vector<pair<int, string> > > args;
   map<int, vector<int> > child;
   istringstream iss(parse);

   while (!iss.eof()) {
      string t;
      getline(iss, t);
      vector<string> field = tokenize(t, '\t', true);
      //ID FORM LEMMA PLEMMA POS PPOS FEAT PFEAT HEAD PHEAD DEPREL PDEPREL FILLPRED PRED APREDs
      int id = stoi(field[0]) - 1;
      tokens.push_back(field[1]);
      int parent = stoi(field[8]);
      if (parent > 0) {
         if (child.find(parent - 1) != child.end()) {
            child[parent - 1].push_back(id);
         } else {
            child[parent - 1] = vector<int>(1, id);
         }
      }
      if (field[13] != "_") {
         preds.push_back(id);
         srlgraph_t::span_type s(id, id + 1);
         srlgraph_t::srlnid_type pid = result.new_pred(s, plabel);
         pids[id] = pid;
      }
      for (int i = 14; i < (int)field.size(); i++) {
         if ((int)args.size() < i - 13) {
            vector<pair<int, string> > a;
            args.push_back(a);
         }
         if (field[i] != "_") {
            args[i - 14].push_back(make_pair(id, field[i]));
         }
      }
   } // while (!iss.eof())

   result.set_tokens(tokens);
   for (int i = 0; i < (int)args.size(); i++) {
      srlgraph_t::srlnid_type pid = pids[preds[i]];
      for (int j = 0; j < (int)args[i].size(); j++) {
         int head = args[i][j].first;
         srlgraph_t::label_type label = args[i][j].second;
         size_t b = head;
         size_t e = head;
         resolve_arg_span(child, head, preds[i], b, e);
         srlgraph_t::span_type s(b, e + 1);
         result.new_arg(pid, s, label);
      }
   }

   return result;
} // read_conll09

void yisi::resolve_arg_span(map<int, vector<int> > child, int curid,
                            srlgraph_t::srlnid_type pid, size_t& b, size_t&e) {
   //cerr << curid << "," << pid << "," << b << "," << e << endl;
   auto curchild = child[curid];
   bool find = false;
   for (auto it = curchild.begin(); it != curchild.end() && !find; it++) {
      if (*it == (int)pid) {
         find = true;
      }
   }
   if (!find) {
      for (auto it = curchild.begin(); it != curchild.end(); it++) {
         if (*it < (int)b) {
            b = *it;
         }
         if (*it > (int)e) {
            e = *it;
         }
         resolve_arg_span(child, *it, pid, b, e);
      }
   }
}

vector<srlgraph_t> yisi::read_conll09batch(string filename) {
   vector<srlgraph_t> result;

   ifstream fin(filename.c_str());
   if (!fin) {
      cerr << "ERROR: Failed to open conll09 parse  file (" << filename << "). Exiting..." << endl;
      exit(1);
   }

   string parse;

   while (!fin.eof()) {
      string line;
      getline(fin, line);
      if (line.empty()) {
         result.push_back(read_conll09(yisi::strip(parse)));
         parse = "";
      } else {
         parse += line + "\n";
      }
      fin.peek();
   }
   return result;
}

srlread_t::srlread_t(string parsefile):parsefile_m(parsefile) {}

vector<srlgraph_t> srlread_t::parse(vector<string> sents) {
   return yisi::read_conll09batch(parsefile_m);
}

vector<srlgraph_t> srltok_t::parse(vector<string> sents) {
   vector<srlgraph_t> result;
   for (auto it = sents.begin(); it != sents.end(); it++) {
      auto tokens = yisi::tokenize(*it);
      result.push_back(srlgraph_t(tokens));
   }
   return result;
}

srlgraph_t srltok_t::parse(string sent) {
   auto tokens = yisi::tokenize(sent);
   auto result = srlgraph_t(tokens);
   return result;
}
