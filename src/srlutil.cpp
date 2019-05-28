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
#include <set>
#include <fstream>
#include <sstream>

using namespace yisi;
using namespace std;

vector<srlgraph_t> yisi::read_srl(vector<sent_t*> sents, string parsefile) {
   // read srl in ASSERT format
   vector<srlgraph_t> result;
   typedef srlgraph_t::span_type span_type;
   typedef srlgraph_t::srlnid_type srlnid_type;

   for (auto it = sents.begin(); it != sents.end(); it++) {
      //vector<string> tokens = tokenize(*it);
      srlgraph_t s(*it);
      result.push_back(s);
   }

   if (parsefile != "") {
      ifstream ifs(parsefile.c_str(), ifstream::in);
      int line_number = 0;
      while (!ifs.eof()) {
         string line;
         getline(ifs, line);
         ++line_number;
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
            if (!(iss >> s)) {
               cerr << "ERROR: bad format found in parse file: " << parsefile
                    << ", line " << line_number << ": " << line << endl;
               break;
            }
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

         if (tmptok.size() > result.at(id).get_sent_length()) {
            //result.at(id).set_tokens(tmptok);
            cerr << "ERROR: Tokenization of words changed by srl. Potential index failure!" << endl;
         }
      } // while (!ifs.eof())
      ifs.close();
   }

   return result;
}  // read_srl

srlgraph_t yisi::read_conll09(string parse, sent_t* sent) {
   srlgraph_t result(sent);
   if (parse.empty()) {
      return result;
   }
   // cerr << result << endl;
   //result.new_root();
   srlgraph_t::label_type plabel = "V";

   vector<string> tokens;
   vector<int> preds;
   vector<srlgraph_t::srlnid_type> p_nids;
   vector<vector<srlgraph_t::label_type> > labels;
   map<int, set<int> > child;
   istringstream iss(parse);

   int n_space = 0;
   while (!iss.eof()) {
      string t;
      getline(iss, t);
      vector<string> field = tokenize(t, '\t', true);
      //ID FORM LEMMA PLEMMA POS PPOS FEAT PFEAT HEAD PHEAD DEPREL PDEPREL FILLPRED PRED APREDs
      int id = stoi(field[0]) - 1 -n_space;
      //cerr << "Reading " << id;
      if (field[1] != ""){
         tokens.push_back(field[1]);
         int p = stoi(field[8]) - n_space;
         if (p > 0) {
            child[p - 1].insert(id);
         }

         for (int i = 14; i < (int)field.size(); i++) {
            if (tokens.size() == 1){
               vector<srlgraph_t::label_type> l;
               l.push_back(field[i]);
               labels.push_back(l);
            } else {
               labels[i-14].push_back(field[i]);
            }
         }
         if (field[13] != "_") {
            preds.push_back(id);
            srlgraph_t::span_type s(id, id + 1);
            srlgraph_t::srlnid_type pid = result.new_pred(s, plabel);
            p_nids.push_back(pid);
            labels[preds.size()-1][id]="V";
         }
         //cerr << " Done." << endl;
      } else {
         n_space++;
         if (field[13] != "_") {
            preds.push_back(-1);
            p_nids.push_back(10000);
         }
      }
   } // while (!iss.eof())

   if (result.get_sent_type() == "word") {
      if (tokens.size() > result.get_sent_length()) {
         if (result.get_sent_length() > 0)
            cerr << "Set tokens rule fired (" << tokens.size() << ","
                 << result.get_sent_length() << ")" << endl;
         result.set_tokens(tokens);
      }
   } else {
      if (result.get_sent_length() > 0 && tokens.size() > result.get_sent_length()) {
         cerr << "ERROR: Tokenization of words changed by srl. Potential index failure!" << endl;
         cerr << "Tokens were: " << join(result.get_sentence(), " ") << endl;
         cerr << "Tokens are: " << join(tokens, " ") << endl;
      }
   }

   for (int i = 0; i < (int)labels.size(); i++) {
      for (int j = 0; j < (int) labels[i].size(); j++){
         populate_label(labels[i], child, j);
      }
   }
   for (int i = 0; i < (int)labels.size(); i++) {
      auto pid = p_nids[i];
      if (pid != 10000) {
         srlgraph_t::span_type curspan;
         srlgraph_t::label_type curlabel = "_";
         for (size_t j = 0; j < labels[i].size(); j++) {
            //cerr << labels[i][j] << " ";
            if (labels[i][j] != curlabel) {
               if (curlabel != "_" && curlabel != "V") {
                  curspan.second = j;
                  result.new_arg(pid, curspan, curlabel);
               }
               curspan.first = j;
               curlabel = labels[i][j];
            }
         }
         if (curlabel != "_" && curlabel != "V") {
            curspan.second = labels[i].size();
            result.new_arg(pid, curspan, curlabel);
         }
         //cerr << endl;
      }
   }
   return result;
} // read_conll09

srlgraph_t yisi::read_conll09(string parse) {
   sent_t* sent = new sent_t("word");

   auto result = read_conll09(parse, sent);

   return result;
} // read_conll09

void yisi::populate_label(vector<string>& labels, map<int, set<int> > child, int i) {
   if (labels[i] != "_" && labels[i] != "V") {
      auto curchildren = child[i];
      for (auto ct = curchildren.begin(); ct != curchildren.end(); ct++) {
         //cerr << "Label " << *ct << " " << labels[*ct] << endl;
         if (labels[*ct] == "_") {
            labels[*ct] = labels[i];
            populate_label(labels, child, *ct);
         }
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
   int i=0;
   while (!fin.eof()) {
      string line;
      getline(fin, line);
      if (line.empty()) {
         result.push_back(read_conll09(yisi::strip(parse)));
         parse = "";
         i++;
      } else {
         parse += line + "\n";
      }
      fin.peek();
   }
   return result;
}

vector<srlgraph_t> yisi::read_conll09batch(string filename, vector<sent_t*> sents) {
   vector<srlgraph_t> result;

   ifstream fin(filename.c_str());
   if (!fin) {
      cerr << "ERROR: Failed to open conll09 parse  file (" << filename << "). Exiting..." << endl;
      exit(1);
   }

   string parse;
   int i=0;
   while (!fin.eof()) {
      string line;
      getline(fin, line);
      if (line.empty()) {
         result.push_back(read_conll09(yisi::strip(parse), sents[i]));
         parse = "";
         i++;
      } else {
         parse += line + "\n";
      }
      fin.peek();
   }
   return result;
}

srlread_t::srlread_t(string parsefile):parsefile_m(parsefile) {}

vector<srlgraph_t> srlread_t::parse(vector<sent_t*> sents) {
   return yisi::read_conll09batch(parsefile_m, sents);
}

vector<srlgraph_t> srltok_t::parse(vector<sent_t*> sents) {
   vector<srlgraph_t> result;
   for (auto it = sents.begin(); it != sents.end(); it++) {
      result.push_back(srlgraph_t(*it));
   }
   return result;
}

srlgraph_t srltok_t::parse(sent_t* sent) {
   auto result = srlgraph_t(sent);
   return result;
}
