/*
 * MIT License
 *
 * Copyright (c) 2020 Guillaume Perez
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

#ifndef SRC_DYNAMICPROG_DYNP
#define SRC_DYNAMICPROG_DYNP

#include <stdio.h>

#include <set>
#include <unordered_map>

#include "Core/MDD.hpp"
#include "Relax/State.hpp"

namespace MDD {

/**
 * Generic class for the representing of dynamic programming problems.
 *
 * \TODO(USER) redefine the behavior of virtuals for particular problems.
 *
 **/
class DynamicProgram {
 public:
  virtual ~DynamicProgram() = default;

 /**
   * Return a state representing the initial state of the DP.
   **/
  virtual State* MakeInitialState() = 0;

  /**
   * State reached by the transition.
   *
   * If transition is prevented, method must return nullptr.
   **/
  virtual State* TransitionFunction(State* s, int value) = 0;

  /**
   * Method for the merging of states.
   *
   * return a state representing the merge of all the states in the
   * set @param states.
   *
   **/
  virtual State* Merge(std::set<State*> states) = 0;
};

}  // namespace MDD

#endif /* SRC_DYNAMICPROG_DYNP */
