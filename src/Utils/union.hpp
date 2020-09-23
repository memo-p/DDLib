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

#ifndef SRC_UTILS_UNION
#define SRC_UTILS_UNION

#include <vector>

namespace MDD {

class unionFind {
  std::vector<int> lead_;
  int size_;

 public:
  unionFind(int size_) : lead_(size_), size_(size_) { clear(); }

  int leader(int v) {
    if (lead_[v] == v) {
      return v;
    }
    return lead_[v] = leader(lead_[v]);
  }

  void setLeader(int v, int l) { lead_[v] = l; }

  void clear() {
    for (int i = 0; i < size_; i++) {
      lead_[i] = 0;
    }
  }
};

inline bool isPermutation(int* t, int size) {
  int* present = new int[size];
  for (int i = 0; i < size; i++) {
    present[i] = 0;
  }
  for (int i = 0; i < size; i++) {
    present[t[i]]++;
    if (present[t[i]] > 1) {
      return false;
    }
  }
  return true;
}
}  // namespace MDD

#endif /* SRC_UTILS_UNION */
