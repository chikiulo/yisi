/**
 * @file maxmatching_test.cpp
 * @brief Unit test for maxmatching.
 *
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include <iostream>

#include "maxmatching.h"

using namespace std;
using namespace yisi;

int main(int argc, char* argv[])
{
   vector<pair<pair<maxmatching_t::id_type, maxmatching_t::id_type>,
         maxmatching_t::weight_type> > r;

   //test case 1
   //square matrix
   // |  a|  b|  c
   //1|1.0|0.0|0.0
   //2|0.0|1.0|0.0
   //3|0.0|0.0|1.0
   maxmatching_t t1;
   t1.add_weight(0, 3, 1.0);
   t1.add_weight(0, 4, 0.0);
   t1.add_weight(0, 5, 0.0);
   t1.add_weight(1, 3, 0.0);
   t1.add_weight(1, 4, 1.0);
   t1.add_weight(1, 5, 0.0);
   t1.add_weight(2, 3, 0.0);
   t1.add_weight(2, 4, 0.0);
   t1.add_weight(2, 5, 1.0);
   r = t1.run();

   cout << "Test case 1: " << endl;
   cout << " |  3|  4|  5" << endl;
   cout << "0|1.0|0.0|0.0" << endl;
   cout << "1|0.0|1.0|0.0" << endl;
   cout << "2|0.0|0.0|1.0" << endl;
   for (size_t i = 0; i < r.size(); i++) {
      cout << "(" << r[i].first.first << "," << r[i].first.second << ","
           << r[i].second << ")" << endl;
   }
   r.clear();

   //test case 2
   //square matrix
   // |  a|  b|  c
   //1|0.7|0.3|0.0
   //2|0.0|0.7|0.3
   //3|0.3|0.0|0.7
   maxmatching_t t2;
   t2.add_weight(0, 3, 0.7);
   t2.add_weight(0, 4, 0.3);
   t2.add_weight(0, 5, 0.0);
   t2.add_weight(1, 3, 0.0);
   t2.add_weight(1, 4, 0.7);
   t2.add_weight(1, 5, 0.3);
   t2.add_weight(2, 3, 0.3);
   t2.add_weight(2, 4, 0.0);
   t2.add_weight(2, 5, 0.7);
   r = t2.run();

   cout << "Test case 2: " << endl;
   cout << " |  3|  4|  5" << endl;
   cout << "0|0.7|0.3|0.0" << endl;
   cout << "1|0.0|0.7|0.3" << endl;
   cout << "2|0.3|0.0|0.7" << endl;
   for (size_t i = 0; i < r.size(); i++) {
      cout << "(" << r[i].first.first << "," << r[i].first.second << ","
           << r[i].second << ")" << endl;
   }
   r.clear();

   //test case 3
   //square matrix
   // |  a|  b|  c
   //1|1.0|1.0|0.8
   //2|0.9|0.8|0.1
   //3|0.9|0.7|0.4
   maxmatching_t t3;
   t3.add_weight(0, 3, 1.0);
   t3.add_weight(0, 4, 1.0);
   t3.add_weight(0, 5, 0.8);
   t3.add_weight(1, 3, 0.9);
   t3.add_weight(1, 4, 0.8);
   t3.add_weight(1, 5, 0.1);
   t3.add_weight(2, 3, 0.9);
   t3.add_weight(2, 4, 0.7);
   t3.add_weight(2, 5, 0.4);
   r = t3.run();

   cout << "Test case 3: " << endl;
   cout << " |  3|  4|  5" << endl;
   cout << "0|1.0|1.0|0.8" << endl;
   cout << "1|0.9|0.8|0.1" << endl;
   cout << "2|0.9|0.7|0.4" << endl;
   for (size_t i = 0; i < r.size(); i++) {
      cout << "(" << r[i].first.first << "," << r[i].first.second << ","
           << r[i].second << ")" << endl;
   }
   r.clear();

   //test case 4
   //rectangular matrix
   // |  a|  b
   //1|0.9|0.9
   //2|0.9|0.8
   //3|0.8|0.1
   //4|1.0|0.1
   maxmatching_t t4;
   t4.add_weight(0, 4, 0.9);
   t4.add_weight(0, 5, 0.9);
   t4.add_weight(1, 4, 0.9);
   t4.add_weight(1, 5, 0.8);
   t4.add_weight(2, 4, 0.8);
   t4.add_weight(2, 5, 0.1);
   t4.add_weight(3, 4, 1.0);
   t4.add_weight(3, 5, 0.1);
   r = t4.run();

   cout << "Test case 4: " << endl;
   cout << " |  4|  5" << endl;
   cout << "0|0.9|0.9" << endl;
   cout << "1|0.9|0.8" << endl;
   cout << "2|0.8|0.1" << endl;
   cout << "3|1.0|0.1" << endl;
   for (size_t i = 0; i < r.size(); i++) {
      cout << "(" << r[i].first.first << "," << r[i].first.second << ","
           << r[i].second << ")" << endl;
   }
   r.clear();

   //test case 5
   //rectangular matrix
   // |  a|  b
   //1|0.9|0.7
   //2|0.8|0.6
   //3|0.7|0.1
   //4|0.9|0.1
   maxmatching_t t5;
   t5.add_weight(0, 4, 0.9);
   t5.add_weight(0, 5, 0.7);
   t5.add_weight(1, 4, 0.8);
   t5.add_weight(1, 5, 0.6);
   t5.add_weight(2, 4, 0.7);
   t5.add_weight(2, 5, 0.1);
   t5.add_weight(3, 4, 0.9);
   t5.add_weight(3, 5, 0.1);
   r = t5.run();

   cout << "Test case 5: " << endl;
   cout << " |  4|  5" << endl;
   cout << "0|0.9|0.7" << endl;
   cout << "1|0.8|0.6" << endl;
   cout << "2|0.7|0.1" << endl;
   cout << "3|0.9|0.1" << endl;
   for (size_t i = 0; i < r.size(); i++) {
      cout << "(" << r[i].first.first << "," << r[i].first.second << ","
           << r[i].second << ")" << endl;
   }
   r.clear();

   return 0;
} // main

