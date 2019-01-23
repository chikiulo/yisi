/**
 * @file maxmatching.cpp
 * @brief Maximum bipartite matching
 *
 * @author Jackie Lo
 *
 * Class implementation of the classes:
 *    - maxmatching_t
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

#include "maxmatching.h"

using namespace yisi;
using namespace std;

maxmatching_t::maxmatching_t(){
   n=0;
   z0_r=0;
   z0_c=0;
   original_length = 0;
   original_width = 0;
}

void maxmatching_t::add_weight(id_type srcid, id_type tgtid, weight_type weight) {
   id_type hid, tid;

   if (src_idx.count(srcid) > 0) {
      hid = src_idx[srcid];
   } else {
      hid = src_idx.size();
      src_idx[srcid] = hid;
      src_rev[hid] = srcid;
      vector<weight_type> v;
      vector<weight_type> e;
      for (id_type i = 0; i < tgt_idx.size(); i++) {
         v.push_back(0.0);
         e.push_back(0.0);
      }
      c_m.push_back(v);
      edge_w.push_back(e);
   }

   if (tgt_idx.count(tgtid) > 0) {
      tid = tgt_idx[tgtid];
   } else {
      tid = tgt_idx.size();
      tgt_idx[tgtid] = tid;
      tgt_rev[tid] = tgtid;
      for (id_type i = 0; i < src_idx.size(); i++) {
         c_m[i].push_back(0.0);
         edge_w[i].push_back(0.0);
      }
   }
   edge_w[hid][tid] = weight;
   c_m[hid][tid] = 1.0 - weight;
}

vector<pair<pair<maxmatching_t::id_type, maxmatching_t::id_type>,
      maxmatching_t::weight_type> > maxmatching_t::run() {

   //patch zero if the cost matrix is not square
   original_length = src_idx.size();
   original_width = tgt_idx.size();
   int ssize = original_length;
   int tsize = original_width;
   while (ssize < tsize) {
      vector<weight_type> v;
      c_m.push_back(v);
      for (int i = 0; i < tsize; i++) {
         c_m[ssize].push_back(0.0);
      }
      ssize++;
   }
   while (tsize < ssize) {
      for (int i = 0; i < ssize; i++) {
         c_m[i].push_back(0.0);
      }
      tsize++;
   }
   n = ssize;

   // run the Munkres algorithm
   for (int i = 0; i < n; i++) {
      row_covered.push_back(false);
      col_covered.push_back(false);
      vector<int> v;
      marked.push_back(v);
      for (int j = 0; j < n; j++) {
         marked[i].push_back(0);
      }
   }

   z0_r = 0;
   z0_c = 0;
   for (int i = 0; i < 2 * n; i++) {
      vector<int> v;
      path.push_back(v);
      for (int j = 0; j < 2 * n; j++) {
         path[i].push_back(0);
      }
   }

   bool done = false;
   int step = 1;
   while (!done) {
      switch (step) {
      case 1:
         step = step1();
         break;
      case 2:
         step = step2();
         break;
      case 3:
         step = step3();
         break;
      case 4:
         step = step4();
         break;
      case 5:
         step = step5();
         break;
      case 6:
         step = step6();
         break;
      default:
         done = true;
      }
   }

   vector<pair<pair<id_type, id_type>, weight_type> > result;

   for (int i = 0; i < original_length; i++) {
      for (int j = 0; j < original_width; j++) {
         if (marked[i][j] == 1) {
            pair<id_type, id_type> e(src_rev[i], tgt_rev[j]);
            pair<pair<id_type, id_type>, weight_type> r(e, edge_w[i][j]);
            result.push_back(r);
         }
      }
   }

   return result;
} // run

/*
 * Munkres algorithm step 1 helper function
 *   For each row of the matrix, find the smallest element
 *   and subtract it from every element in its row.
 *   Go to step 2 at the end.
 */
int maxmatching_t::step1() {
   for (int i = 0; i < n; i++) {
      double minval = 100.0;
      for (int j = 0; j < n; j++) {
         if (c_m[i][j] < minval) {
            minval = c_m[i][j];
         }
      }
      for (int j = 0; j < n; j++) {
         c_m[i][j] -= minval;
      }
   }
   return 2;
}

/*
 * Munkres algorithm step 2 helper function
 *   Find a zero (Z) in the resulting matrix.
 *   If there is no starred zero in its row or column, star Z.
 *   Repeat for each element in the matrix.
 *   Go the step 3 at the end.
 */
int maxmatching_t::step2() {
   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
         if (c_m[i][j] == 0.0 && !row_covered[i] && !col_covered[j]) {
            marked[i][j] = 1;
            row_covered[i] = true;
            col_covered[j] = true;
         }
      }
   }
   //clear covers
   for (int i = 0; i < n; i++) {
      row_covered[i] = false;
      col_covered[i] = false;
   }
   return 3;
}

/*
 * Munkres algorithm step 3 helper function
 *   Cover each column containing a starred zero.
 *   If K columns are covered,
 *   the starred zeros describe a complete set of unique assignments.
 *   In this case, go to DONE, otherwise, go to step 4 at the end.
 */
int maxmatching_t::step3() {
   int count = 0;
   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
         if (marked[i][j] == 1) {
            col_covered[j] = true;
            count += 1;
         }
      }
   }
   if (count >= n) {
      return 7;
   } else {
      return 4;
   }
}

/*
 * Munkres algorithm step 4 helper function
 *   Find a uncovered zero and prime it.
 *   If there is no starred zero in the two containing this primed zero,
 *   go to step 5,
 *   otherwise, cover this row and
 *   uncover the column containing the starred zero.
 *   Continue in this manner until there are no uncovered zeros left.
 *   Save the smallest uncovered vallue and go to step 6.
 */
int maxmatching_t::step4() {
   int step = 0;
   bool done = false;
   int row = -1;
   int col = -1;
   int star_col = -1;
   while (!done) {
      find_a_zero(row, col);
      if (row < 0) {
         done = true;
         step = 6;
      } else {
         marked[row][col] = 2;
         star_col = find_star_in_row(row);
         if (star_col >= 0) {
            col = star_col;
            row_covered[row] = true;
            col_covered[col] = false;
         } else {
            done = true;
            z0_r = row;
            z0_c = col;
            step = 5;
         }
      }
   }
   return step;
}

/*
 * Munkres algorithm step 5 helper function
 *   Construct a series of alternating primed and starred zeros as follows:
 *   Let Z0 represent the uncovered primed zero found in step 4.
 *   Let Z1 denote the started zero in the column of Z0 (if any).
 *   Let Z2 denote the primed zero in the row of Z1 (there will always be one.
 *   Continue until the series teminates at a primed zero
 *   that has not starred zero in its column.
 *   Unstar each starred zero of the series,
 *   star each primed zero of the series,
 *   erase all orimes and uncover every line in the matrix.
 *   Go to step 3.
 */
int maxmatching_t::step5() {
   int count = 0;
   path[count][0] = z0_r;
   path[count][1] = z0_c;
   bool done = false;
   while (!done) {
      int row = find_star_in_col(path[count][1]);
      if (row >= 0) {
         count += 1;
         path[count][0] = row;
         path[count][1] = path[count - 1][1];
      } else {
         done = true;
      }
      if (!done) {
         int col = find_prime_in_row(path[count][0]);
         count += 1;
         path[count][0] = path[count - 1][0];
         path[count][1] = col;
      }
   }

   //convert path
   for (int i = 0; i < count + 1; i++) {
      if (marked[path[i][0]][path[i][1]] == 1) {
         marked[path[i][0]][path[i][1]] = 0;
      } else {
         marked[path[i][0]][path[i][1]] = 1;
      }
   }
   //clear covers & erase primes
   for (int i = 0; i < n; i++) {
      row_covered[i] = false;
      col_covered[i] = false;
      for (int j = 0; j < n; j++) {
         if (marked[i][j] == 2) {
            marked[i][j] = 0;
         }
      }
   }
   return 3;
}

/*
 * Munkres algorithm step 6 helper function
 *   Add the value found in step 4 to every element of each covered row,
 *   and subtract it from every element of each uncovered column.
 *   Go to step 4 without altering any stars, primes, or covered lines.
 */
int maxmatching_t::step6() {
   double minval = find_smallest();
   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
         if (row_covered[i]) {
            c_m[i][j] += minval;
         }
         if (!col_covered[j]) {
            c_m[i][j] -= minval;
         }
      }
   }
   return 4;
}

/*
 * Munkres algorithm helper function for finding min val in cost matrix
 *   Find the smallest uncovered value in the matrix.
 */
double maxmatching_t::find_smallest() {
   double minval = 1.0;
   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
         if (!row_covered[i] && !col_covered[j]) {
            if (minval > c_m[i][j]) {
               minval = c_m[i][j];
            }
         }
      }
   }
   return minval;
}

/*
 * Munkres algorithm helper function
 *   Find the first uncovered element with value 0.
 */
void maxmatching_t::find_a_zero(int& r, int& c) {
   r = -1;
   c = -1;
   for (int i = 0; i < n && r == -1; i++) {
      for (int j = 0; j < n; j++) {
         if (c_m[i][j] == 0 && !row_covered[i] && !col_covered[j]) {
            r = i;
            c = j;
         }
      }
   }
}

/*
 * Munkres algorithm helper function
 *   Find the first starred element in the specified row.
 *   Returns the column index, or -1 if no starred element was found.
 */
int maxmatching_t::find_star_in_row(int row) {
   int col = -1;
   for (int j = 0; j < n && col == -1; j++) {
      if (marked[row][j] == 1) {
         col = j;
      }
   }
   return col;
}

/*
 * Munkres algorithm helper function
 *   Find the first starred element in the specified col.
 *   Returns the row index, or -1 if no starred element was found.
 */
int maxmatching_t::find_star_in_col(int col) {
   int row = -1;
   for (int i = 0; i < n && row == -1; i++) {
      if (marked[i][col] == 1) {
         row = i;
      }
   }
   return row;
}

/*
 * Munkres algorithm helper function
 *   Find the first prime element in the specified row.
 *   Returns the column index, or -1 if no prime element was found.
 */
int maxmatching_t::find_prime_in_row(int row) {
   int col = -1;
   for (int j = 0; j < n && col == -1; j++) {
      if (marked[row][j] == 2) {
         col = j;
      }
   }
   return col;
}
