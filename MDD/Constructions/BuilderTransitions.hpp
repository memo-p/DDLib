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

#ifndef SRC_CONSTRUCTIONS_BUILDERTRANSITIONS
#define SRC_CONSTRUCTIONS_BUILDERTRANSITIONS

#include <stdio.h>
#include <unordered_map>
#include <cassert>

#include "Constructions/MDDBuilder.hpp"
#include "Core/MDD.hpp"
#include "Operations/Reduce.hpp"

namespace MDD {

/**
 * Builder of MDD using as input the MDD as a transition graph.
 * Transition are of defined as follow:
 * - t[0] = starting Node
 * - t[1] = value
 * - t[2] = ending Node
 * 
 * Warning: Root node must be labeled 0. 
 * Transitions must be given in the order of a search in the MDD.
 * This implies that before using a node i, at least 1 transition leading to 
 * i must exists.
 * Only transition from level i to level i+1 are allowed.
 **/
class MDDBuilderFromTransition : public MDDBuilder {
 public:
  MDDBuilderFromTransition(std::vector<std::vector<int>> const &transitions,
                           int nb_vars)
      : transitions_(transitions), nb_vars_(nb_vars) {}

  MDD *Build();

 private:

  const std::vector<std::vector<int>> &transitions_;
  const int nb_vars_;
  static const int start_id;
  static const int value_id;
  static const int end_id;
};

}  // namespace MDD

#endif /* SRC_CONSTRUCTIONS_BUILDERTRANSITIONS */
