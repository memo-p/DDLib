/*
 * MIT License
 *
 * Copyright (c) 2018 Guillaume Perez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SRC_OPERATIONS_REDUCE
#define SRC_OPERATIONS_REDUCE

#include <stdio.h>

#include <queue>

#include "Core/MDD.hpp"
#include "DataStructures/SparseSet.hpp"
#include "Utils/ChronoP.hpp"
#include "Utils/union.hpp"

namespace MDD {

class Reduce {
  MDD &mdd_;

  int largest_layer_;
  int largest_domain_;
  int radix_size_;

  unionFind leaders_1_;
  unionFind leaders_2_;

  std::vector<int> layer_ids_1_;        // Permutation used for the raddix.
  std::vector<int> layer_ids_2_;        // Permutation used for the raddix.
  std::vector<Node *> layer_nodes_;     // store the current layer of Nodes
  std::vector<Arc *> layer_arcs_;       // and the currently processed Arcs
  std::vector<int> layer_values_;       // value used for the raddix
  std::vector<int> raddix_count_;       // counter for the raddix
  std::vector<int> raddix_cumulative_;  // cumulative for the raddix
  std::vector<int> non_null_raddix_;    // non-zero raddix count entry

  Node escape_node_;
  Arc escape_edge_;

  int layer_size;
  int currentLayerSize;
  Node *n, *n_next;
  Arc *e, *e_next;
  bool raddix_mode;
  int over;
  int delta;
  int id;
  int id_prev;
  bool swap_status;

  std::vector<int> &LayerID() {
    return (swap_status) ? layer_ids_1_ : layer_ids_2_;
  }
  std::vector<int> &LayerIDNext() {
    return (swap_status) ? layer_ids_2_ : layer_ids_1_;
  }
  unionFind &Leader() { return (swap_status) ? leaders_1_ : leaders_2_; }
  unionFind &LeaderNext() { return (swap_status) ? leaders_2_ : leaders_1_; }

  int ArcsUpdateRadixInit(int layer);

  void RadixCount(std::vector<int> &l_id);

  void RadixCumulative();

  void Permutation(std::vector<int> &l_id_1, std::vector<int> &l_id_2);

  void LeadManagement(unionFind &lead_1, unionFind &lead_2,
                      std::vector<int> &l_id_1, std::vector<int> &l_id_2);

  void PrepareNextRadixLabel();

  void PrepareNextRadixNode();

  void Merge(std::vector<int> &layer_id, unionFind &leader);

 public:
  Reduce(MDD &mdd);
};

}  // namespace MDD

#endif /* SRC_OPERATIONS_REDUCE */
