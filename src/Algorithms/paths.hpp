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

#ifndef SRC_ALGORITHMS_PATHS
#define SRC_ALGORITHMS_PATHS

#include <stdio.h>

#include <iostream>
#include <unordered_map>

#include "Core/MDD.hpp"
#include "Operations/Search.hpp"

namespace MDD {

/**
 * Class that process the longest path value in the MDD
 **/
class LongestPathBFS : public BreadthFirstSearch {
 private:
  std::unordered_map<int, int> longest_past_;

 public:
  LongestPathBFS(MDD &mdd) : BreadthFirstSearch(mdd) {}

  void OnArc(Arc *a) override {
    longest_past_[a->End()->UID()] =
        std::max(longest_past_[a->End()->UID()],
                 longest_past_[a->Start()->UID()] + a->Value());
  }
  /**
   * Return the number of tuples processed.
   **/
  int GetLongestPath() { return longest_past_[GetMDD().Final()->UID()]; }
};

inline int LongestPath(MDD &mdd) {
  LongestPathBFS lp(mdd);
  lp.Run();
  return lp.GetLongestPath();
}

}  // namespace MDD

#endif /* SRC_ALGORITHMS_PATHS */
