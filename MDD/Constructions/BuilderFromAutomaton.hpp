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

#ifndef SRC_CONSTRUCTIONS_BUILDERFROMAUTOMATON
#define SRC_CONSTRUCTIONS_BUILDERFROMAUTOMATON

#include <stdio.h>

#include "Constructions/MDDBuilder.hpp"
#include "Constructions/MDDBuilderGrid.hpp"
#include "Core/MDD.hpp"
#include "Operations/Reduce.hpp"

namespace MDD {

/**
 * Class used to build an MDD from an automaton.
 * * Transition are of defined as follow:
 * - t[0] = starting Node
 * - t[1] = value
 * - t[2] = ending Node
 * 
 * Note that all the states must be positive or null integers.
 **/
class MDDBuilderAutomaton : public MDDBuilder {
 public:
  MDDBuilderAutomaton(std::vector<std::vector<int>> const &transitions,
                      int nb_vars, int start, std::vector<int> finals)
      : transitions_(transitions),
        nb_vars_(nb_vars),
        start_(start),
        finals_(finals) {}

  MDD *Build();

 private:
  void ExtractNumberOfStates();

  const std::vector<std::vector<int>> &transitions_;
  const int nb_vars_;
  int start_;
  std::vector<int> finals_;
  int nb_states_;
  
  static const int start_id;
  static const int value_id;
  static const int end_id;
};

}  // namespace MDD

#endif /* SRC_CONSTRUCTIONS_BUILDERFROMAUTOMATON */
