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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SRC_RELAX_PARTITIONERS_STATEPARTITIONER
#define SRC_RELAX_PARTITIONERS_STATEPARTITIONER

#include <stdio.h>

#include <queue>
#include <random>
#include <set>
#include <vector>

#include "Relax/Creation/DPRelaxCreation.hpp"
#include "Relax/Partitioner.hpp"

namespace MDD {

/**
 * Partitioning of nodes for state construction.
 * 
 * This is currently not usable since the reduction
 * does not automatically maintains the states.
 **/
class StatePartitioner : public Partitioner {
 private:
  DynamicProgRelaxCreation *dpc_;

 public:
  StatePartitioner(DynamicProgRelaxCreation *dpc) : dpc_(dpc) {}

  /**
   * Return the DP currently being build.
   **/
  DynamicProgRelaxCreation &DPC() { return *dpc_; }

  State *GetState(Node *n) { return DPC().States().at(n->UID()); }
};

/**
 * The @param width -1 largest nodes are kept, and all the other merged.
 **/
class MaxRankPartitioner : public StatePartitioner {
 private:
  std::priority_queue<Node *> nodes_;

 public:
  MaxRankPartitioner(DynamicProgRelaxCreation *dpc) : StatePartitioner(dpc) {}

  void Partition(const DblList<Node> &layer,
                 std::vector<std::set<Node *>> &partition) override {
    assert(layer.Size() > 1);
    const uint64_t max_width = partition.size();
    assert(max_width > 0);
    auto gt = [this](Node *e1, Node *e2) {
      return GetState(e1)->Rank() > GetState(e2)->Rank();
    };
    std::priority_queue<Node *, std::vector<Node *>, decltype(gt)> queue(gt);
    Node *n = layer.First();
    while (n) {
      queue.push(n);
      n = n->Next();
    }
    uint64_t i = 0;
    while (!queue.empty()) {
      n = queue.top();
      queue.pop();
      partition[i].insert(n);
      if (i + 1 < max_width) {
        i++;
      }
    }
  }
};

}  // namespace MDD

#endif /* SRC_RELAX_PARTITIONERS_STATEPARTITIONER */
