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

#ifndef SRC_DYNAMICPROG_DYNSUM
#define SRC_DYNAMICPROG_DYNSUM

#include <stdio.h>
#include <set>
#include <unordered_map>

#include <Core/MDD.hpp>
#include <DynamicProg/DynP.hpp>
#include <Relax/State.hpp>

namespace MDD {

/**
 * Dynamic programming sum.
 *
 **/
class SumDynProg : public DynamicProgram {
  int v_max_;
  std::unordered_map<int, IntState*> existing_states_;

 public:
  SumDynProg(int v_max = 2) : v_max_(v_max) {}

  State* MakeInitialState() override {
    if (!existing_states_[0]) {
      existing_states_[0] = new IntState(0);
    }
    return existing_states_[0];
  }

  State* TransitionFunction(State* s, int value) override {
    IntState* si = static_cast<IntState*>(s);
    const int v = (*si) + value;
    if (v <= v_max_) {
      if (existing_states_[v]) {
        return existing_states_[v];
      }
      return existing_states_[v] = new IntState(v);
    }
    return nullptr;
  }

  State* Merge(std::set<State*> states) override {
    int v_min = std::numeric_limits<int>().max();
    for (auto&& s : states) {
      IntState* si = static_cast<IntState*>(s);
      v_min = std::min(v_min, si->Value());
    }
    if (existing_states_[v_min]) {
      return existing_states_[v_min];
    }
    return existing_states_[v_min] = new IntState(v_min);
  }
};

}  // namespace MDD

#endif /* SRC_DYNAMICPROG_DYNSUM */
