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

#ifndef SRC_OPERATIONS_REDUCEDFSMAP
#define SRC_OPERATIONS_REDUCEDFSMAP

#include <unordered_map>

#include "Core/MDD.hpp"
#include "Operations/Search.hpp"

namespace MDD {
/**
 * Checker that the MDD is reduced.
 **/
class ReduceDFSMap : public DepthFirstSearch {
 private:
  std::unordered_map<std::string, Node *> same_;

 public:
  ReduceDFSMap(MDD &mdd) : DepthFirstSearch(mdd) {}

  /**
   * Maintain the map of equivalent node and
   * update the newV field representing the unique
   * representative of all the equivalent nodes.
   **/
  void OnCloseNode(Node *n) override {
    if (same_[n->to_string()] == nullptr) {
      same_[n->to_string()] = n;
    }
    n->newV_ = same_[n->to_string()];
  }
  /**
   * Update the terminating node of the arc.
   **/
  void OnCloseArc(Arc *a) override {
    a->Start()->updateArc(a, a->End()->newV_);
  }
};
}  // namespace MDD
#endif /* SRC_OPERATIONS_REDUCEDFSMAP */
