/**
 * @file srlmate.cpp
 * @brief SRL MATE
 *
 * @author Jackie Lo
 *
 * Class implementation of:
 *    - srlmate (wrapper class for MATE srl tool)
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "srlmate.h"
#include "srlutil.h"
#include "util.h"

#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

using namespace yisi;
using namespace std;

#define JNI_SAFE_CALL(name, jni_env_ptr, expr)              \
auto name = jni_env_ptr->expr;                              \
{                                                           \
   bool failed = name == nullptr;                           \
   if (jni_env_ptr->ExceptionOccurred()) {                  \
      jni_env_ptr->ExceptionDescribe();                     \
      failed = true;                                        \
   }                                                        \
   if (failed) {                                            \
      throw std::runtime_error("JNI-call to Java failed."); \
   }                                                        \
}                                                           \

JavaVM* srlmate_t::jvm_m = NULL;
JNIEnv* srlmate_t::jen_m = NULL;
int srlmate_t::obj_cnt_m = 0;

srlmate_t::srlmate_t(string path) {
   cerr << "Setting up MATE ...";

   ifstream fin(path.c_str());
   if (!fin) {
      cerr << "ERROR: Failed to open MATE config file (" << path << "). Exiting..." << endl;
      exit(1);
   }

   string yisi_home = "";
   string mate_jars = "";
   string lang = "";
   string token = "";
   string morph = "";
   string lemma = "";
   string tagger = "";
   string parser = "";
   string srl = "";
   bool rerank = false;
   bool hybrid = false;

   while (!fin.eof()) {
      string line;
      getline(fin, line);
      istringstream iss(line);
      string cfgn, cfgv;
      getline(iss, cfgn, '=');
      getline(iss, cfgv);
      if (cfgn == "yisi_home") {
         yisi_home = cfgv;
      }
      else if (cfgn == "mate_jars") {
         mate_jars = cfgv;
      }
      else if (cfgn == "lang") {
         lang = cfgv;
      }
      else if (cfgn == "rerank") {
         if ((cfgv.compare("0") == 0) || (cfgv.compare("false") == 0)) {
            rerank = false;
         } else {
            rerank = true;
         }
      }
      else if (cfgn == "hybrid") {
         if ((cfgv.compare("0") == 0) || (cfgv.compare("false") == 0)) {
            hybrid = false;
         } else {
            hybrid = true;
         }
      }
      else if (cfgn == "token") {
         token = cfgv;
      }
      else if (cfgn == "morph") {
         morph = cfgv;
      }
      else if (cfgn == "lemma") {
         lemma = cfgv;
      }
      else if (cfgn == "tagger") {
         tagger = cfgv;
      }
      else if (cfgn == "parser") {
         parser = cfgv;
      }
      else if (cfgn == "srl") {
         srl = cfgv;
      }
   }

   // init JVM
   if (jvm_m == NULL) {
      size_t opt_count = 2;
      JavaVMOption* vm_opts = new JavaVMOption[opt_count];
      string opt0 = "-Djava.class.path=" + yisi_home + "/obj/srlmate.jar";
      string opt1 = "-Xmx12g";
      vm_opts[0].optionString = const_cast<char*>(opt0.c_str());
      vm_opts[1].optionString = const_cast<char*>(opt1.c_str());

      JavaVMInitArgs vm_args;
      vm_args.version = JNI_VERSION_1_6;
      vm_args.options = vm_opts;
      vm_args.nOptions = opt_count;

      jint result = JNI_CreateJavaVM(&jvm_m, (void**)&jen_m, &vm_args);
      delete vm_opts;
      if (result != JNI_OK) {
         cerr << "ERROR: Failed to create Java VM. (error code = " << result << "). Exiting..." << endl;
         exit(1);
         jvm_m = NULL;
      }
   }
   ++obj_cnt_m;

   JNI_SAFE_CALL(mcls, jen_m, FindClass("yisi/Mate"));
   JNI_SAFE_CALL(ctor, jen_m, GetMethodID(mcls, "<init>", "()V"));
   JNI_SAFE_CALL(mobj, jen_m, NewObject(mcls, ctor));
   JNI_SAFE_CALL(init, jen_m, GetMethodID(mcls, "init",
      "(Ljava/lang/String;Ljava/lang/String;ZZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;"
      "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"));
   JNI_SAFE_CALL(jerr, jen_m,
                 CallObjectMethod(mobj,
                                  init,
                                  jen_m->NewStringUTF(mate_jars.c_str()),
                                  jen_m->NewStringUTF(lang.c_str()),
                                  rerank ? JNI_TRUE : JNI_FALSE,
                                  hybrid ? JNI_TRUE : JNI_FALSE,
                                  jen_m->NewStringUTF(token.c_str()),
                                  jen_m->NewStringUTF(morph.c_str()),
                                  jen_m->NewStringUTF(lemma.c_str()),
                                  jen_m->NewStringUTF(parser.c_str()),
                                  jen_m->NewStringUTF(tagger.c_str()),
                                  jen_m->NewStringUTF(srl.c_str())));
   string error(jen_m->GetStringUTFChars((jstring)jerr, NULL));
   if (!error.empty()) {
      cerr << "ERROR: Failed to initialize yisi.Mate (" << error << "). Exiting..." << endl;
      exit(1);
   }

   mate_class_m = mcls;
   mate_object_m = mobj;
   cerr << "Done." << endl;
}  // srlmate_t

srlmate_t::~srlmate_t() {
   --obj_cnt_m;
   if (obj_cnt_m == 0 && jvm_m != NULL) {
      jvm_m->DestroyJavaVM();
      jvm_m = NULL;
      jen_m = NULL;
   }
}

string srlmate_t::noparse(vector<string> tokens) {
   string result = "";
   for (size_t i = 0; i < tokens.size(); i++) {
      //ID FORM LEMMA PLEMMA POS PPOS FEAT PFEAT HEAD PHEAD DEPREL PDEPREL FILLPRED PRED APREDs
      result += to_string(i+1) + "\t" + tokens[i] + "\t--\t--\t_\t_\t_\t_\t"
                + to_string(i) + "\t" + to_string(i) + "\t--\t--\t_\t_\n";
   }
   return yisi::strip(result);
}

string srlmate_t::jrun(string sent) {
   string result = "";
   vector<string> tokens = yisi::tokenize(sent);

   if (!sent.empty() && tokens.size() <= 100) {
      JNI_SAFE_CALL(methid, jen_m,
                    GetMethodID(mate_class_m, "parse",
                                "(Ljava/lang/String;)Ljava/lang/String;"));
      try {
         JNI_SAFE_CALL(jparse, jen_m,
                       CallObjectMethod(mate_object_m, methid,
                                        jen_m->NewStringUTF(sent.c_str())));
         result = jen_m->GetStringUTFChars((jstring)jparse, NULL);
      } catch (...) {
         result += noparse(tokens);
      }
   } else {
      result += noparse(tokens);
   }
   return result;
}

srlgraph_t srlmate_t::parse(string sent) {
   string srl_str = jrun(sent);
   srlgraph_t result = read_conll09(srl_str);
   return result;
}

vector<srlgraph_t> srlmate_t::parse(vector<string> sents) {
   //batch srl-ing
   vector<srlgraph_t> result;
   for (auto it = sents.begin(); it != sents.end(); it++) {
      result.push_back(parse(*it));
   }
   return result;
}

