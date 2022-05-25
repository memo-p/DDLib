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

#ifndef SRC_DATASTRUCTURES_SPARSESET
#define SRC_DATASTRUCTURES_SPARSESET

#include <cstdio>

namespace MDD {

class Sparse_Set {
 public:
  Sparse_Set(int size)
      : sparse_(new int[size]),
        dense_(new int[size]),
        size_(0),
        sparseSize_(size) {
    for (int i = 0; i < size; ++i) {
      sparse_[i] = -1;
      dense_[i] = -1;
    }
  }
  ~Sparse_Set() {
    delete[] sparse_;
    delete[] dense_;
  }

  int operator[](int i) { return dense_[i]; }
  int Element(int i) { return dense_[i]; }
  int Index(int e) { return sparse_[e]; }

  bool member(int k) {
    int a = sparse_[k];
    if (a < size_ && a >= 0 && dense_[a] == k) {
      return true;
    }
    return false;
  }

  void add(int k) {
    int a = sparse_[k];
    int b = size_;
    if (a < 0 || a >= b || dense_[a] != k) {
      sparse_[k] = b;
      dense_[b] = k;
      size_++;
    }
  }

  void remove(int k) {
    if (member(k)) {
      swap(k, dense_[size_ - 1]);
      size_--;
    }
  }

  void clear() { size_ = 0; }
  void setSize(int newValue) { size_ = newValue; }
  int size() { return size_; }

  void swap(int k, int L) {
    int sk = sparse_[k];
    int sL = sparse_[L];
    dense_[sk] = L;
    sparse_[L] = sk;
    sparse_[k] = sL;
    dense_[sL] = k;
  }

 private:
  int* sparse_;
  int* dense_;
  int size_;
  int sparseSize_;
};

}  // namespace MDD


#endif /* SRC_DATASTRUCTURES_SPARSESET */
