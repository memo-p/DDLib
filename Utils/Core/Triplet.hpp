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

#ifndef SRC_CORE_TRIPLET
#define SRC_CORE_TRIPLET

#include <stdio.h>
#include <iostream>
#include "Core/MDD.hpp"
#include <set>

namespace MDD {

/**
 * @brief Base class for the MDD operation.
 * 
 * it is used to represent couples of nodes from
 * distinct MDDs during operations.
 * 
 */
class Triplet {
 public:
  static thread_local Triplet *allocated;
  void *operator new(std::size_t count) {
    if (allocated == nullptr) {
      char *m = (char *)::new char[count * NBALLOC];
      Triplet *e = (Triplet *)m;
      allocated = (Triplet *)m;
      for (int i = 0; i < NBALLOC - 1; i++) {
        e->next_ = (Triplet *)(m + count * (i + 1));
        e = e->next_;
      }
      e->next_ = nullptr;
    }
    Triplet *retour = allocated;
    allocated = allocated->next_;
    return retour;
  }
  void operator delete(void *ptr) {
    Triplet *ptrn = (Triplet *)ptr;
    ptrn->next_ = allocated;
    allocated = ptrn;
  }

  Triplet(Node *x, Node *x1, Node *x2) : x_(x), x1_(x1), x2_(x2), next_(NULL) {}
  Node *x_;
  Node *x1_;
  Node *x2_;
  Triplet *next_;
};

}  // namespace MDD


#endif /* SRC_CORE_TRIPLET */
