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

#ifndef MDD_BUILDERS_GRID
#define MDD_BUILDERS_GRID

#include <stdio.h>
#include "Core/MDD.hpp"
#include "Builders/base.hpp"
#include "Operations/Reduce.hpp"

namespace MDD {

class GridMDDBuilder : public MDDBuilder {
  void deleteBadNode();

 public:
  GridMDDBuilder(int size, int numState);
  void addTransition(int start, int value, int end);
  void addLastTransition(int start, int value, int end);
  void addStartingTransition(int value, int end);
  void addEndingTransition(int start, int value);
  void addEndingLastTransition(int start, int value);

  MDD* Build();

 private:
  MDD* mdd_;
  int numState_;
  int size_;
  int nb_vals_;
  std::vector<std::vector<Node*>> grid_;
};

}  // namespace MDD

#endif /* MDD_BUILDERS_GRID */
