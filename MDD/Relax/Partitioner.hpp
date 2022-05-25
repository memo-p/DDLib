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

#ifndef SRC_RELAX_PARTITIONNER
#define SRC_RELAX_PARTITIONNER

#include <stdio.h>

#include <random>
#include <set>
#include <vector>

#include "Core/MDD.hpp"

namespace MDD {

/**
 * Base class for the definition of partitioning of nodes
 **/
class Partitioner {
 public:
  virtual ~Partitioner() {}
  /**
   * Function that select the nodes to merge.
   *
   * Default is to put all the first Width()-1 alone
   * and all the other in one partition.
   *
   * //TODO(user) redefine the behavior of this method.
   **/
  virtual void Partition(const DblList<Node> &layer,
                         std::vector<std::set<Node *>> &partition) {
    assert(layer.Size() > 1);
    const size_t max_width = partition.size();
    assert(max_width > 0);
    Node *n = layer.First();
    uint64_t i = 0;
    while (n) {
      partition[i].insert(n);
      if (i + 1 < max_width) {
        i++;
      }
      n = n->Next();
    }
  }
};

/**
 * Random Partitioner. ?odes will be split uniformly at random in partitions.
 **/
class RandomPartitioner : public Partitioner {
 public:
  void Partition(const DblList<Node> &layer,
                 std::vector<std::set<Node *>> &partition) override {
    assert(layer.Size() > 1);
    const uint64_t max_width = partition.size();
    assert(max_width > 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> distribution(0, max_width - 1);

    Node *n = layer.First();
    while (n) {
      uint64_t i = distribution(generator);
      partition[i].insert(n);
      n = n->Next();
    }
  }
};

}  // namespace MDD

#endif /* SRC_RELAX_PARTITIONNER */
