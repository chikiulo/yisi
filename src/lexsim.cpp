/**
 * @file lexsim.cpp
 * @brief Lexical similarity
 *
 * @author Jackie Lo
 *
 * Class implementation for the classes:
 *    - lexsimexact_t (simple exact match lex sim model)
 *    - lexsimibm_t (ibm1 lex sim model)
 *    - lexsimw2v_t (w2v lex sim model)
 *    - lexsimemapw2v_t (emap lex sim model for YiSi-2)
 *    - lexsimbiw2v_t (bilingual w2v lex sim model for YiSi-3)
 *    - lexsim_t (wrapper class)
 * and the definitions of some utility functions working on it.
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "lexsim.h"

#include <fstream>
#include <math.h>
#include <algorithm>

using namespace yisi;
using namespace std;

double lexsimexact_t::get_sim(string ref, string hyp, int mode) {
   if (mode == yisi::INP_MODE) {
      cerr << "ERROR: exact matching lex sim model is not defined "
           << "in crosslingual settings. Exiting..." << endl;
      exit(1);
   }
   if (ref == hyp) {
      return 1.0;
   } else {
      return eps_m;
   }
}

double lexsimlcs_t::get_sim(string ref, string hyp, int mode) {
   if (mode == yisi::INP_MODE) {
      cerr << "ERROR: longest common subsequence lex sim model is not defined "
         << "in crosslingual settings. Exiting..." << endl;
      exit(1);
   }
   double lcs_n = 0.0;
   size_t ref_n = ref.length();
   size_t hyp_n = hyp.length();
   // find the length of the longest common character subsequence
   for (size_t i = 0; i < ref_n - lcs_n; i++) {
      // cerr << "Current ref pos: " << i << endl;
      size_t j;
      for (j = lcs_n + 1; j <= ref_n - i; j++) {
         //cerr << "Previous common length: " << lcs_n << endl;
         //cerr << "Current common length: " << j << endl;
         auto searching = ref.substr(i, j);
         //cerr << "Current target: " << searching << endl;
         auto found = hyp.find(searching);
         //cerr << "Found position: " << found << endl;
         if (found == string::npos) {
            break;
         }
      }
      j = j - 1;
      if (j > lcs_n) {
         lcs_n = j;
      }
   }
   //cerr << "lcs of (" << ref << " , " << hyp << ") = ("
   //     << lcs_n << "," << ref_n << "," << hyp_n << ")" << endl;
   return (2.0 * lcs_n) / (double)(ref_n + hyp_n);
}

//lexsimibm_t::lexsimibm_t(string path) {
//   cerr << "Reading ibm model from " << path << endl;
//
//   ifstream PFGE(path.c_str());
//   if (!PFGE) {
//      cerr << "ERROR: fail to open ibm model. Exiting..." << endl;
//      exit(1);
//   }
//   while (!PFGE.eof()) {
//      string ref;
//      string hyp;
//      double pfge;
//      PFGE >> hyp >> ref >> pfge;
//      pfge_m[pair<string, string>(ref, hyp)] = pfge;
//   }
//   PFGE.close();
//   cerr << "Finished reading." << endl;
//
//   string pegf_path(path);
//   size_t l = pegf_path.length();
//   string f = pegf_path.substr(l - 11, 2);
//   string e = pegf_path.substr(l - 2, 2);
//
//   pegf_path.replace(l - 11, 2, e);
//   pegf_path.replace(l - 2, 2, f);
//
//   cerr << "Reading ibm model from " << pegf_path << endl;
//   ifstream PEGF(pegf_path.c_str());
//   if (!PEGF) {
//      cerr << "ERROR: fail to open ibm model. Exiting..." << endl;
//      exit(1);
//   }
//   while (!PEGF.eof()) {
//      string hyp;
//      string ref;
//      double pegf;
//      PEGF >> ref >> hyp >> pegf;
//      pegf_m[pair<string, string>(hyp, ref)] = pegf;
//   }
//   PEGF.close();
//   cerr << "Finished reading." << endl;
//}

//double lexsimibm_t::get_sim(string ref, string hyp) {
//   double pfge = eps_m;
//   double pegf = eps_m;
//   if (pfge_m.find(pair<string, string>(ref, hyp)) != pfge_m.end()) {
//      pfge = pfge_m[pair<string, string>(ref, hyp)];
//   }
//   if (pegf_m.find(pair<string, string>(hyp, ref)) != pegf_m.end()) {
//      pegf = pegf_m[pair<string, string>(hyp, ref)];
//   }
//   return (pfge + pegf) / 2.0; //TODO: really? should it be GM instead of AM?
//}

lexsimw2v_t::lexsimw2v_t(string path) {
   if (path.substr(path.size() - 3) == "bin") {
      read_binw2v(path, outembeddings_m, dimension_m);
   } else {
      read_txtw2v(path, outembeddings_m, dimension_m);
   }
   if (outembeddings_m.find("<unk>") == outembeddings_m.end()) {
      vector<double> unk;
      //for (int i = 0; i < dimension_m; i++) {
      //   unk.push_back(eps_m);
      //}
      outembeddings_m["<unk>"] = unk;
   }
   func_m = "cosine";
}

//lexsimw2v_t::lexsimw2v_t(string path, string func) {
//   if (path.substr(path.size() - 3) == "bin") {
//      read_binw2v(path, outembeddings_m, dimension_m);
//   } else {
//      read_txtw2v(path, outembeddings_m, dimension_m);
//   }
//   if (outembeddings_m.find("<unk>") == outembeddings_m.end()) {
//      vector<double> unk;
//      outembeddings_m["<unk>"] = unk;
//   }
//   func_m = func;
//}

vector<double>& lexsimw2v_t::get_wv(string word, int mode) {
   if (mode == yisi::INP_MODE) {
      cerr << "ERROR: w2v lex sim model is not defined "
           << "in crosslingual settings. Exiting..." << endl;
      exit(1);
   }
   return yisi::get_wv(outembeddings_m, word);
}

double lexsimw2v_t::get_sim(string ref, string hyp, int mode) {
   if (mode == yisi::INP_MODE) {
      cerr << "ERROR: w2v lex sim model is not defined "
           << "in crosslingual settings. Exiting..." << endl;
      exit(1);
   }
   if (lowercase(ref) == lowercase(hyp)) {
      return 1.0;
   } else {
      return this->get_sim(this->get_wv(ref, mode), this->get_wv(hyp, yisi::HYP_MODE));
   }
}

double lexsimw2v_t::get_sim(vector<double>& ref, vector<double>& hyp) {
   if ((int)ref.size() == dimension_m && (int)hyp.size() == dimension_m) {
      return yisi::get_sim(ref, hyp, func_m);
   } else {
      return 0.0;
   }
}

void lexsimw2v_t::write_txtw2v(std::string path) {
   cerr << "Writing w2v model in text format to file " << path << " ...";
   ofstream os(path.c_str());

   if (!os) {
      cerr << "ERROR: Failed to open file to write w2v model. No model written." << endl;
   } else {
      os << outembeddings_m.size() - 1 << " " << dimension_m << endl;
      for (auto it = outembeddings_m.begin(); it != outembeddings_m.end(); it++) {
         if (it->first != "<unk>") {
            os << it->first;
            auto ds = it->second;
            for (auto jt = ds.begin(); jt != ds.end(); jt++) {
               os << " " << *jt;
            }
            os << endl;
         }
      }
   }

   os.close();
   cerr << "Done." << endl;
}

lexsimemapw2v_t::lexsimemapw2v_t(string emap_path, string outw2v_path) :
   lexsimw2v_t(outw2v_path) {
   cerr << "Reading emap model from " << emap_path << endl;
   ifstream EMAP(emap_path.c_str());
   if (!EMAP) {
      cerr << "ERROR: fail to open emap model. Exiting..." << endl;
      exit(1);
   }
   while (!EMAP.eof()) {
      string inp;
      string hyp;
      EMAP >> inp >> hyp;
      emap_m[inp] = hyp;
   }
   EMAP.close();
   cerr << "Finished reading." << endl;
}

vector<double>& lexsimemapw2v_t::get_wv(string word, int mode) {
   if (mode == yisi::INP_MODE) {
      if (emap_m.find(word) != emap_m.end()) {
         word = emap_m[word];
      } else if (emap_m.find(lowercase(word)) != emap_m.end()) {
         word = emap_m[lowercase(word)];
      }
   }
   return yisi::get_wv(outembeddings_m, word);
}

double lexsimemapw2v_t::get_sim(string s1, string hyp, int mode) {
   if (lowercase(s1) == lowercase(hyp)) {
      return 1.0;
   } else {
      double result = this->get_sim(this->get_wv(s1, mode), this->get_wv(hyp, yisi::HYP_MODE));
      //cerr << "(" << s1 << "," << hyp << "," << mode << "," << result << ")" << endl;
      return result;
   }
}

double lexsimemapw2v_t::get_sim(vector<double>& s1, vector<double>& hyp) {
   if ((int)s1.size() == dimension_m && (int)hyp.size() == dimension_m) {
      return yisi::get_sim(s1, hyp, func_m);
   } else {
      return 0.0;
   }
}

lexsimbiw2v_t::lexsimbiw2v_t(string inpw2v_path, string outw2v_path)
: lexsimw2v_t(outw2v_path) {
   if (inpw2v_path.substr(inpw2v_path.size() - 3) == "bin") {
      read_binw2v(inpw2v_path, inpembeddings_m, dimension_m);
   } else {
      read_txtw2v(inpw2v_path, inpembeddings_m, dimension_m);
   }
   if (inpembeddings_m.find("<unk>") == inpembeddings_m.end()) {
      vector<double> unk;
      //for (int i = 0; i < dimension_m; i++) {
      //   unk.push_back(eps_m);
      //}
      inpembeddings_m["<unk>"] = unk;
   }
}

vector<double>& lexsimbiw2v_t::get_wv(string word, int mode) {
   if (mode == yisi::INP_MODE) {
      return yisi::get_wv(inpembeddings_m, word);
   } else {
      return yisi::get_wv(outembeddings_m, word);
   }
}

double lexsimbiw2v_t::get_sim(string s1, string hyp, int mode) {
   if ((lowercase(s1) == lowercase(hyp))
      && (((mode == yisi::INP_MODE)
         && ((inpembeddings_m.find(s1) == inpembeddings_m.end())
            || outembeddings_m.find(hyp) == outembeddings_m.end()))
         || (mode != yisi::INP_MODE))) {
      return 1.0;
   } else {
      double result = this->get_sim(this->get_wv(s1, mode), this->get_wv(hyp, yisi::HYP_MODE));
      //cerr << "(" << s1 << "," << hyp << "," << mode << "," << result << ")" << endl;
      return result;
   }
}

double lexsimbiw2v_t::get_sim(vector<double>& s1, vector<double>& hyp) {
   if ((int)s1.size() == dimension_m && (int)hyp.size() == dimension_m) {
      return yisi::get_sim(s1, hyp, func_m);
   } else {
      return 0.0;
   }
}

double lexsimemb_t::get_sim(string s1, string hyp, int mode){
  cerr <<"ERROR: lexsim model is a contextual embedding model, cannot compute lexsim without providing the embedding. Exiting..." << endl;
  exit(1);
}

double lexsimemb_t::get_sim(vector<double>& s1, vector<double>& hyp){
  return yisi::get_sim(s1, hyp, func_m);
}

lexsim_t::lexsim_t() {
   lexsim_p = new lexsimexact_t();
}

lexsim_t::lexsim_t(string name, string out_path, string inp_path) {
   if (name == "exact") {
      lexsim_p = new lexsimexact_t();
   } else if (name == "ibm") {
     //lexsim_p = new lexsimibm_t(path);
   } else if (name == "w2v") {
     lexsim_p = new lexsimw2v_t(out_path);
   } else if (name == "ibmw2v" || name == "emapw2v") {
     lexsim_p = new lexsimemapw2v_t(inp_path, out_path);
   } else if (name == "biw2v") {
     lexsim_p = new lexsimbiw2v_t(inp_path, out_path);
   } else if (name == "lcs") {
     lexsim_p = new lexsimlcs_t();
   } else if (name == "emb"){
     lexsim_p = new lexsimemb_t();
   } else {
     cerr << "ERROR: Unknown lexsim model type " << name << endl;
   }
   lexsim_name_m = name;
   outlexsim_path_m = out_path;
   inplexsim_path_m = inp_path;
}

lexsim_t::lexsim_t(lexsim_t& rhs) {
   if (rhs.lexsim_name_m == "exact") {
      lexsim_p = new lexsimexact_t();
   } else if (rhs.lexsim_name_m == "ibm") {
      //lexsim_p = new lexsimibm_t(rhs.lexsim_path_m);
   } else if (rhs.lexsim_name_m == "w2v") {
      lexsim_p = new lexsimw2v_t(rhs.outlexsim_path_m);
   } else if (rhs.lexsim_name_m == "ibmw2v" || rhs.lexsim_name_m == "emapw2v") {
     lexsim_p = new lexsimemapw2v_t(rhs.inplexsim_path_m, rhs.outlexsim_path_m);
   } else if (rhs.lexsim_name_m == "biw2v") {
      lexsim_p = new lexsimbiw2v_t(rhs.inplexsim_path_m, rhs.outlexsim_path_m);
   } else if (rhs.lexsim_name_m == "lcs") {
      lexsim_p = new lexsimlcs_t();
   } else if (rhs.lexsim_name_m == "emb") {
     lexsim_p = new lexsimemb_t();
   }
   lexsim_name_m = rhs.lexsim_name_m;
   outlexsim_path_m = rhs.outlexsim_path_m;
   inplexsim_path_m = rhs.inplexsim_path_m;
}

lexsim_t::~lexsim_t() {
   if (lexsim_p != NULL) {
      delete lexsim_p;
   }
}

double lexsim_t::get_sim(string s1, string hyp, int mode) {
   //cerr << "Querying " << mode << " lex sim of " << s1 << " and " << hyp << "; ";
   if (mode == yisi::INP_MODE) {
      if (xlscache_m.find(s1) != xlscache_m.end()) {
         if (xlscache_m[s1].find(hyp) != xlscache_m[s1].end()) {
            auto s = xlscache_m[s1][hyp];
            return s;
         }
      } else {
         map<string, double> c;
         xlscache_m[s1] = c;
      }
   } else {
      if (mlscache_m.find(s1) != mlscache_m.end()) {
         if (mlscache_m[s1].find(hyp) != mlscache_m[s1].end()) {
            auto s = mlscache_m[s1][hyp];
            //cerr << "find in cache = " << s << endl;
            return s;
         }
      } else {
         map<string, double> c;
         mlscache_m[s1] = c;
      }
   }
   //cerr << "computing = ";
   double s = lexsim_p->get_sim(s1, hyp, mode);
   if (mode == yisi::INP_MODE) {
      xlscache_m[s1][hyp] = s;
   } else {
      mlscache_m[s1][hyp] = s;
      //cerr<<s << endl;
   }
   return s;
}

double lexsim_t::get_sim(vector<double>& v1, vector<double>& hyp) {
   return lexsim_p->get_sim(v1, hyp);
}

vector<double>& lexsim_t::get_wv(string word, int mode) {
   return lexsim_p->get_wv(word, mode);
}
void yisi::read_binw2v(string path, map<string, vector<double> >& model, int& dimension) {
   long long n = 0;
   long long d = 0;
   char tmp;

   cerr << "Reading w2v binary model from " << path << endl;
   ifstream W2V(path.c_str(), ios::in | ios::binary);
   if (!W2V) {
      cerr << "ERROR: Failed to open w2v model. Exiting..." << endl;
      exit(1);
   }

   W2V >> n;
   W2V >> d;
   dimension = (int)d;
   cerr << "Size of voc: " << n << " Dimension: " << dimension << endl;
   W2V.get(tmp);
   for (long long i = 0; i < n; i++) {
      string wordtmp;
      getline(W2V, wordtmp, ' ');
      string word = strip(wordtmp);
      //cerr << i << "\t" << word << endl;
      vector<double> embedding;
      double len = 0.0;
      for (long long j = 0; j < dimension; j++) {
         float f;
         W2V.read((char*)&f, sizeof(float));
         //cerr << "\t" << j << " " << f << endl;
         embedding.push_back(f);
         len += f * f;
      }
      len = sqrt(len);
      //cerr << len << endl;
      for (long long j = 0; j < dimension; j++) {
         embedding[j] /= len;
      }

      model[word] = embedding;
      //getline(W2V, word, '\n');
   }

   cerr << "Finished reading w2v model. Embedding size is " << model.size() << endl;

   W2V.close();
}

void yisi::read_txtw2v(string path, map<string, vector<double> >& model, int& dimension) {
   unsigned int n = 0;
   unsigned int d = 0;
   char tmp;

   cerr << "Reading w2v text model from " << path << endl;
   ifstream W2V(path.c_str());
   if (!W2V) {
      cerr << "ERROR: Failed to open w2v model. Exiting..." << endl;
      exit(1);
   }

   W2V >> n;
   W2V >> d;
   dimension = (int)d;
   cerr << "Size of voc: " << n << " Dimension: " << dimension << endl;
   W2V.get(tmp);
   for (unsigned int i = 0; i < n; i++) {
      string word;
      getline(W2V, word, ' ');
      //cout << i << ": [" << word << ", ";
      vector<double> embedding;
      double len = 0.0;
      string af;
      float f;
      for (int j = 0; j < dimension - 1; j++) {
         getline(W2V, af, ' ');
         f = atof(af.c_str());
         //cout << f << ", ";
         embedding.push_back(f);
         len += f * f;
      }
      getline(W2V, af);
      f = atof(af.c_str());
      //cout << f << "]" << endl;
      embedding.push_back(f);

      len += f * f;

      len = sqrt(len);
      //cerr << len << endl;
      for (int j = 0; j < dimension; j++) {
         embedding[j] /= len;
      }

      model[word] = embedding;
   }
   cerr << "Finished reading w2v model." << endl;
   W2V.close();
}

vector<double>& yisi::get_wv(map<string, vector<double> >& model, string word) {
   if (model.find(word) != model.end()) {
      return model[word];
   } else {
      string l = lowercase(word);
      if (model.find(l) != model.end()) {
         return model[l];
      } else {
         return model["<unk>"];
      }
   }
}

double yisi::get_sim(vector<double>& v1, vector<double>& v2, string func) {
   return simfunc(func, v1, v2);
}

bool yisi::sort_helper(pair<string, double> i, pair<string, double> j) {
   return i.second > j.second;
}

double yisi::simfunc(string funcname, vector<double>& ref, vector<double>& hyp) {
   if (funcname == "ucosine") {
      return cosine(ref, hyp, 0);
   } else if (funcname == "cosine") {
      return cosine(ref, hyp, 1);
   } else if (funcname == "tcosine") {
      return cosine(ref, hyp, 2);
   } else if (funcname == "jaccard") {
      return jaccard(ref, hyp, 0);
   } else if (funcname == "mjaccard") {
      return jaccard(ref, hyp, 1);
   } else {
      cerr << "ERROR: Unknown lexsim function " << funcname << endl;
      exit(1);
   }
}

double yisi::cosine(vector<double>& ref, vector<double>& hyp, int mode) {
   // grounded cosine similarity mapping all resulted values ranged from [-1,0) to 0
   double sim = 0.0;

   if (ref.size() != hyp.size()) {
      return sim;
   }
   for (size_t i = 0; i < ref.size(); i++) {
      sim += ref[i] * hyp[i];
   }
   if (mode == 0) {
      // original cosine similarity
      return sim;
   } else if (mode == 1) {
      // grounded cosine similarity mapping all resulted values ranged from [-1,0) to 0
      if (sim > 0.0) {
         return sim;
      } else {
         return 0.0;
      }
   } else {
      // transformed cosine similarity: [-1,1] * 0.5 + 0.5
      return sim * 0.5 + 0.5;
   }
}


double yisi::jaccard(vector<double>& ref, vector<double>& hyp, int mode) {
   double min = 0.0;
   double max = 0.0;

   for (size_t i = 0; i < ref.size(); i++) {
      if (mode == 0 or (ref[i] >= 0 and hyp[i] >= 0)) {
         if (ref[i] >= hyp[i]) {
            min += hyp[i];
            max += ref[i];
         } else {
            min += ref[i];
            max += hyp[i];
         }
      } else if (mode == 1) {
         double mref = ref[i];
         double mhyp = hyp[i];
         if (mref < 0) {
            mref *= -1;
         }
         if (mhyp < 0) {
            mhyp *= -1;
         }
         max += mref + mhyp;
      }
   }
   return min / max;
}
