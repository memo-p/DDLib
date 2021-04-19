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

#ifndef SRC_RELAX_PARTITIONERS_SETPARTITIONER
#define SRC_RELAX_PARTITIONERS_SETPARTITIONER

#include <stdio.h>

#include <array>
#include <queue>
#include <random>
#include <set>
#include <vector>

#include "Relax/Partitioners/StatePartitioner.hpp"
#include "dkm.hpp"

namespace MDD {

/**
 * The @param width -1 largest nodes are kept, and all the other merged.
 **/
template<class StateCreation>
class SmallKMeansPartitioner : public StatePartitioner<StateCreation> {
 private:
 public:
  SmallKMeansPartitioner(StateCreation *dpc)
      : StatePartitioner<StateCreation>(dpc) {}

  void Partition(const DblList<Node> &layer,
                 std::vector<std::set<Node *>> &partition) override {
    assert(layer.Size() > 1);
    const uint64_t max_width = partition.size();
    assert(max_width > 0);

    if (static_cast<uint64_t>(layer.Size()) < max_width) {
      Partitioner::Partition(layer, partition);
      return;
    }
    
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> distribution(0, max_width - 1);

    std::vector<SmallBitsState *> states;
    std::vector<Node *> nodes;

    // data extraction
    Node *n = layer.First();
    while (n) {
      auto r1 = StatePartitioner<StateCreation>::GetState(n);
      if (r1 == nullptr) {  // no info
        uint64_t i = distribution(generator);
        partition[i].insert(n);
      } else {
        states.emplace_back(static_cast<SmallBitsState*>(r1));
        nodes.push_back(n);
      }
      n = n->Next();
    }

    if (nodes.size() < max_width) {
      for (size_t i = 0; i < states.size(); i++) {
        partition[i].insert(nodes[i]);
      }
      return;
    }
    // Kmeans
    std::vector<std::array<float,64>> data;
    for (size_t i = 0; i < states.size(); i++) {
      data.emplace_back();
      for (size_t elem = 0; elem < 64; elem++)
      {
        if (states[i]->Value() & SetBit(0ul, elem)) {
          data.back()[elem] = 1.;
        }
      }
    }
    auto cluster_data = dkm::kmeans_lloyd(data, static_cast<uint32_t>(max_width)); 
    auto labels = std::get<1>(cluster_data);
    for (size_t i = 0; i < states.size(); i++) {
      partition[labels[i]].insert(nodes[i]);
    }
  }
};

}  // namespace MDD

#endif /* SRC_RELAX_PARTITIONERS_SETPARTITIONER */
