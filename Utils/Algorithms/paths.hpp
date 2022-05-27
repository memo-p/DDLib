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

#ifndef SRC_ALGORITHMS_PATHS
#define SRC_ALGORITHMS_PATHS

#include <stdio.h>
#include <limits>

#include <iostream>
#include <unordered_map>
#include <functional>
#include <utility>

#include "Core/MDD.hpp"
#include "Operations/Search.hpp"

namespace MDD {

/**
 * Class that process the longest path value in the MDD
 **/
class LongestPathBFS : public BreadthFirstSearch {
 private:
  std::unordered_map<int, int> longest_past_;
  std::function<int(Arc *)> value_;

 public:
  LongestPathBFS(
      MDD &mdd,
      std::function<int(Arc *)> value = [](Arc *a) { return a->Value(); })
      : BreadthFirstSearch(mdd), value_(value) {}

  void OnArc(Arc *a) override {
    longest_past_[a->End()->UID()] =
        std::max(longest_past_[a->End()->UID()],
                 longest_past_[a->Start()->UID()] + value_(a));
  }
  /**
   * Return the Longest Path size.
   **/
  int GetLongestPath() { return longest_past_[GetMDD().Final()->UID()]; }
};

inline int LongestPath(MDD &mdd) {
  LongestPathBFS lp(mdd);
  lp.Run();
  return lp.GetLongestPath();
}


/**
 * Class that process the shortest path value in the MDD
 **/
class ShortestPathBFS : public BreadthFirstSearch {
 private:
  std::unordered_map<int, int> shortest_past_;
  std::function<int(Arc *)> value_;

 public:
  ShortestPathBFS(
      MDD &mdd,
      std::function<int(Arc *)> value = [](Arc *a) { return a->Value(); })
      : BreadthFirstSearch(mdd), value_(value) {}

  void OnArc(Arc *a) override {
    int end_id = a->End()->UID();
    if (shortest_past_.find(end_id) == shortest_past_.end()) {
      shortest_past_[end_id] = std::numeric_limits<int>::max();
    }
    
    shortest_past_[end_id] =
        std::min(shortest_past_[end_id],
                 shortest_past_[a->Start()->UID()] + value_(a));
  }
  /**
   * Return the shortest Path size.
   **/
  int GetShortestPath() { return shortest_past_[GetMDD().Final()->UID()]; }
};

inline int ShortestPath(MDD &mdd) {
  ShortestPathBFS lp(mdd);
  lp.Run();
  return lp.GetShortestPath();
}

}  // namespace MDD

#endif /* SRC_ALGORITHMS_PATHS */
