/**
 * @file maxmatching.h
 * @brief Maximum bipartite matching
 *
 * @author Jackie Lo
 *
 * Class definition of maximum bipartite matching algorithm:
 *    - maxmatching_t
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#ifndef MAXMATCHING_H
#define MAXMATCHING_H

#include <utility>
#include <vector> 
#include <map>

namespace yisi {

   class maxmatching_t {
   public:
      typedef size_t id_type;
      typedef double weight_type;

      maxmatching_t();

      void add_weight(id_type srcid, id_type tgtid, weight_type weight);

      std::vector<std::pair<std::pair<id_type, id_type>, weight_type> > run();

   private:
      std::vector< std::vector<double> > c_m; // cost matrix
      std::vector<bool> row_covered;
      std::vector<bool> col_covered;
      int original_length;
      int original_width;
      int n;
      int z0_r;
      int z0_c;

      std::vector< std::vector<int> > marked;
      std::vector< std::vector<int> > path;

      std::map<id_type, id_type> src_idx;
      std::map<id_type, id_type> src_rev;
      std::map<id_type, id_type> tgt_idx;
      std::map<id_type, id_type> tgt_rev;
      std::vector< std::vector<double> > edge_w;

      int step1();
      int step2();
      int step3();
      int step4();
      int step5();
      int step6();
      double find_smallest();
      void find_a_zero(int& row, int& col);
      int find_star_in_row(int row);
      int find_star_in_col(int col);
      int find_prime_in_row(int row);
   }; //class maxmatching_t

} // yisi

#endif
