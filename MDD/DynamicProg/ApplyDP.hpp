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

#ifndef SRC_DYNAMICPROG_APPLYDP
#define SRC_DYNAMICPROG_APPLYDP

#include <stdio.h>

#include <list>
#include <set>
#include <unordered_map>

#include "Core/MDD.hpp"
#include "DynamicProg/DynP.hpp"
#include "Relax/Creation/DPRelaxCreation.hpp"
#include "Relax/State.hpp"
#include "Operations/Apply.hpp"

namespace MDD {

class ApplyState : public State {
 private:
  /**
   * Set of nodes currently used for mdd1
   *
   * note that the nodes are ordered using
   * a default orderer (http://www.cplusplus.com/reference/set/set/)
   **/
  std::set<Node*> s1_;
  /**
   * Set of nodes currently used for mdd2
   **/
  std::set<Node*> s2_;

 public:
  ApplyState(std::set<Node*> s1, std::set<Node*> s2) : s1_(s1), s2_(s2) {}

  std::string to_string() override { return to_string(s1_, s2_); }

  static std::string to_string(std::set<Node*> const& s1,
                               std::set<Node*> const& s2) {
    std::ostringstream oss;
    for (Node* n : s1) {
      oss << n->UID() << "-";
    }
    oss << "+";
    for (Node* n : s2) {
      oss << n->UID() << "-";
    }
    return oss.str();
  }

  std::set<Node*>& S1() { return s1_; }
  std::set<Node*>& S2() { return s2_; }
};

/**
 * Relax Apply operation as a dynamic program.
 *
 * This should be used as test, since it might not
 * be efficient because of the lack of incrementality.
 **/
class ApplyDP : public DynamicProgram {
  MDD& mdd1_;
  MDD& mdd2_;
  const unsigned int op_;

  std::unordered_map<std::string, ApplyState*> states_map_;
  std::list<std::unique_ptr<ApplyState>> states_;

  std::set<Node*> buffer_s1_;
  std::set<Node*> buffer_s2_;

  /**
   * Current operation being processed.
   * Note that the dynamic Program is supposed to be developped in a BFS
   * fashion.
   **/
  unsigned int op_not_not_;
  unsigned int op_not_yes_;
  unsigned int op_yes_not_;
  unsigned int op_yes_yes_;

 public:
  ApplyDP(MDD& mdd1, MDD& mdd2, unsigned int op)
      : mdd1_(mdd1),
        mdd2_(mdd2),
        op_(op),
        op_not_not_(Apply::op_not_not_all_),
        op_not_yes_(Apply::op_not_yes_all_),
        op_yes_not_(Apply::op_yes_not_all_),
        op_yes_yes_(Apply::op_yes_yes_all_) {}

  State* MakeInitialState() override {
    states_.emplace_back(new ApplyState(std::set<Node*>(), std::set<Node*>()));
    states_.back()->S1().emplace(mdd1_.Root());
    states_.back()->S2().emplace(mdd2_.Root());
    states_map_[states_.back()->to_string()] = states_.back().get();
    return states_.back().get();
  }

  State* TransitionFunction(State* s, int value) override {
    ClearBuffers();
    ApplyState* as = static_cast<ApplyState*>(s);
    int var_index = -1;
    for (Node* n : as->S1()) {
      var_index = n->VarIndex();
      Arc* a = n->get(value);
      if (a) {
        buffer_s1_.insert(a->End());
      }
    }
    for (Node* n : as->S2()) {
      var_index = n->VarIndex();
      Arc* a = n->get(value);
      if (a) {
        buffer_s2_.insert(a->End());
      }
    }
    if (var_index == mdd1_.getSize() - 1) {
      op_not_not_ = Apply::op_not_not_last_;
      op_not_yes_ = Apply::op_not_yes_last_;
      op_yes_not_ = Apply::op_yes_not_last_;
      op_yes_yes_ = Apply::op_yes_yes_last_;
    }
    if (buffer_s1_.empty()) {
      if (buffer_s2_.empty()) {
        return (op_not_not_ & op_) ? GetStateForBuffer() : nullptr;
      } else {
        return (op_not_yes_ & op_) ? GetStateForBuffer() : nullptr;
      }
    } else {
      if (buffer_s2_.empty()) {
        return (op_yes_not_ & op_) ? GetStateForBuffer() : nullptr;
      } else {
        return (op_yes_yes_ & op_) ? GetStateForBuffer() : nullptr;
      }
    }
  }

  State* Merge(std::set<State*> states) override {
    ClearBuffers();
    for (State* s : states) {
      ApplyState* as = static_cast<ApplyState*>(s);
      AddStateToBuffer(as);
    }
    return GetStateForBuffer();
  }

 private:
  void ClearBuffers() {
    buffer_s1_.clear();
    buffer_s2_.clear();
  }

  void AddStateToBuffer(ApplyState* as) {
    for (Node* n : as->S1()) {
      buffer_s1_.insert(n);
    }
    for (Node* n : as->S2()) {
      buffer_s2_.insert(n);
    }
  }

  State* GetStateForBuffer() {
    auto signature = ApplyState::to_string(buffer_s1_, buffer_s2_);
    auto se = states_map_[signature];
    if (se) {
      return se;
    }
    states_.emplace_back(new ApplyState(buffer_s1_, buffer_s2_));
    states_map_[signature] = states_.back().get();
    return states_.back().get();
  }
};

inline MDD* RelaxApplyDP(MDD& mdd1, MDD& mdd2, int domain, unsigned int op,
                         int width, int depth) {
  ApplyDP adp(mdd1, mdd2, op);
  Partitioner p;
  DynamicProgRelaxCreation dp_ap(mdd1.getSize(), domain, &adp, &p, width,
                                 depth);
  return dp_ap.Build();
}

}  // namespace MDD

#endif /* SRC_DYNAMICPROG_APPLYDP */
