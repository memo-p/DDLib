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

#ifndef SRC_DYNAMICPROG_CONSTRAINTS_ALLDIFFERENT
#define SRC_DYNAMICPROG_CONSTRAINTS_ALLDIFFERENT

#include <stdio.h>
#include <memory>
#include <set>
#include <unordered_map>

#include <Core/MDD.hpp>
#include <DynamicProg/DynP.hpp>
#include <Relax/State.hpp>

namespace MDD {

/**
 * Dynamic programing encoding of the alldifferent constraint.
 *
 **/
class AllDifferentDP : public DynamicProgram {
  /**
   * Number of possible values of the alldifferent.
   **/
  int nb_values_;
  /**
   * Initial state of the DP.
   **/
  std::unique_ptr<State> initial_state_;
  /**
   * Does the set fit an uint64_t
   **/
  bool is_small_;

  /**
   * buffer of the existing states.
   **/
  std::unordered_map<std::string, std::shared_ptr<State>> existing_states_;

 public:
  AllDifferentDP(int nb_values)
      : nb_values_(nb_values), is_small_(nb_values_ <= 64) {
    if (is_small_) {
      initial_state_ =
          std::unique_ptr<SmallBitsState>(new SmallBitsState(~0ul));
    } else {
      initial_state_ =
          std::unique_ptr<SetState>(new SetState(nb_values_, true));
    }
  }
  /**
   * Return a state representing the initial state of the DP.
   **/
  State* MakeInitialState() override { return initial_state_.get(); }

  /**
   * State reached by the transition.
   **/
  State* TransitionFunction(State* s, int value) override {
    std::shared_ptr<State> reached_state;
    if (is_small_) {
      SmallBitsState* s_small = static_cast<SmallBitsState*>(s);
      if (s_small->Value() & OneBit(value)) {
        reached_state =
            std::make_shared<SmallBitsState>(UnsetBit(s_small->Value(), value));
      } else {
        return nullptr;
      }
    } else {
      SetState* s_set = static_cast<SetState*>(s);
      if (!s_set->Contains(value)) {
        return nullptr;
      }

      std::shared_ptr<SetState> ns =
          std::make_shared<SetState>(nb_values_, true);
      ns->Intersection(*s_set);
      ns->Remove(value);
      reached_state = ns;
    }
    std::shared_ptr<State>& map_s =
        existing_states_[reached_state->to_string()];
    if (!map_s) {
      map_s = reached_state;
    }
    return map_s.get();
  }

  /**
   * Method for the merging of states.
   **/
  State* Merge(std::set<State*> states) override{
    std::shared_ptr<State> reached_state;
    if (is_small_) {
      std::shared_ptr<SmallBitsState> small_new =
          std::make_shared<SmallBitsState>(0ul);
      for (State* s : states) {
        SmallBitsState* s_small = static_cast<SmallBitsState*>(s);
        small_new->Set(small_new->Value() | s_small->Value());
      }
      reached_state = small_new;
    } else {
      std::shared_ptr<SetState> ns =
          std::make_shared<SetState>(nb_values_, false);
      for (State* s : states) {
        SetState* s_small = static_cast<SetState*>(s);
        ns->Union(*s_small);
      }
      reached_state = ns;
    }
    std::shared_ptr<State>& map_s =
        existing_states_[reached_state->to_string()];
    if (!map_s) {
      map_s = reached_state;
    }
    return map_s.get();
  }
};

}  // namespace MDD

#endif /* SRC_DYNAMICPROG_CONSTRAINTS_ALLDIFFERENT */
