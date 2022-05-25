/*
 * MIT License
 *
 * Copyright (c) 2021 Guillaume Perez
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

#ifndef SRC_DYNAMICPROG_SETCOVER
#define SRC_DYNAMICPROG_SETCOVER

#include <stdio.h>
#include <limits>
#include <set>
#include <unordered_map>

#include <Core/MDD.hpp>
#include <DynamicProg/DynP.hpp>
#include <Relax/State.hpp>

namespace MDD {

/**
 * Class representing the state for the Set Cover problem.
 **/
class SetCoverState : public State {
 private:
  IntState is_;
  SetState bs_;
  // Shortest path of a node,
  int sp_;

 public:
  SetCoverState(int is, uint64_t nb_rows_, bool full = false)
      : is_(is), bs_(nb_rows_, full), sp_(std::numeric_limits<int>::max()) {}

  std::string to_string() override {
    return is_.to_string() +"+"+ bs_.to_string();
  };

  int Node() { return is_.Value(); }
  int SetNode(int new_node) { return is_ = new_node; }
  BitSet& Set() { return bs_; }

  // Shortest path of a node,
  int SP() const { return sp_; }
  void UpdateSP(int sp) { sp_ = std::min(sp_, sp); }

  int Rank() override { return SP(); }
};

/**
 * Dynamic programming Set Cover problem.
 *
 **/
class SetCoverDP : public DynamicProgram {
  std::unordered_map<std::string, SetCoverState*> existing_states;
  /**
   * For each variable, the rows in which it appears.
   **/
  std::vector<BitSet> const& rows_membership_;
  /**
   * For each variable, the rows in which it doesn't appear
   **/
  std::vector<BitSet> rows_not_;
  /**
   * For each row, the last (lexico) member variable.
   **/
  std::vector<size_t> last_support_;
  /**
   * For each variable, the row for which the variable is
   * the last support.
   **/
  std::vector<std::vector<size_t>> last_support_inv_;
  /**
   * Number of rows (constraints/sets) in the problem.
   **/
  int64_t nb_rows_;
  /**
   * Cost associated to each variable.
   **/
  std::vector<int> costs_;
  /**
   * Temporary state for transition function.
   **/
  SetCoverState buffer_;

 public:
  SetCoverDP(std::vector<BitSet> const& rows_membership,
               std::vector<int> costs)
      : rows_membership_(rows_membership),
        nb_rows_(-1),
        costs_(costs),
        buffer_(0, rows_membership[0].MaxNbElements())
         {
    for (size_t i = 0; i < rows_membership_.size(); i++) {
      nb_rows_ = rows_membership_[i].MaxNbElements();
      rows_not_.emplace_back(nb_rows_, false);
      rows_not_.back().Union(rows_membership_[i]);
      rows_not_.back().Not();
    }
    if (nb_rows_ < 0) {
      std::cerr << "no rows detected for graph coloring problem \n";
      exit(1);
    }

    last_support_.resize(nb_rows_, 0);
    for (size_t i = 0; i < rows_membership_.size(); i++) {
      for (int r = 0; r < nb_rows_; r++) {
        if (rows_membership_[i].Contains(r)) {
          last_support_[r] = i;
        }
      }
    }
    last_support_inv_.resize(costs.size());
    for (int r = 0; r < nb_rows_; r++) {
      last_support_inv_[last_support_[r]].push_back(r);
    }
  }

  SetCoverDP(std::vector<BitSet> const& rows_membership)
      : SetCoverDP(rows_membership,
                     std::vector<int>(rows_membership.size(), 1)) {}

  State* MakeInitialState() override {
    return new SetCoverState(0, nb_rows_, true);
  }

  State* TransitionFunction(State* s, int value) override {
    SetCoverState* ss = static_cast<SetCoverState*>(s);
    const int var = ss->Node();
    BitSet& bs = ss->Set();
    if (value) {
      buffer_.Set().Clear();
      buffer_.Set().Union(bs);
      buffer_.Set().Intersection(rows_not_[var]);
      return GetBufferState(var + 1, ss->SP() + costs_[var]);
    } else {
      if (CanBeZero(var, bs)) {
        buffer_.Set().Clear();
        buffer_.Set().Union(bs);
        return GetBufferState(var + 1, ss->SP());
      }
      return nullptr;
    }
  }

  /**
   * @brief Checks if the curent node can output a zero value.
   **/
  bool CanBeZero(const int var, BitSet& bs) {
    for (size_t i = 0; i < last_support_inv_[var].size(); i++) {
      if (bs.Contains(last_support_inv_[var][i])) {
        return false;
      }
    }

    return true;
  }

  State* Merge(std::set<State*> states) override {
    buffer_.Set().Clear();
    assert(states.size() > 0);
    SetCoverState* ss1 = static_cast<SetCoverState*>(*states.begin());
    buffer_.Set().Union(ss1->Set());
    int shortest_path = std::numeric_limits<int>::max();
    for (auto&& s : states) {
      SetCoverState* si = static_cast<SetCoverState*>(s);
      buffer_.Set().Union(si->Set());
      shortest_path = std::min(shortest_path, si->SP());
    }
    return GetBufferState(ss1->Node(), shortest_path);
  }

 protected:
  SetCoverState* GetBufferState(int node, int path_value) {
    buffer_.SetNode(node);
    auto s = buffer_.to_string();
    if (existing_states[s]) {
      auto ss = existing_states[s];
      ss->UpdateSP(path_value);
      return ss;
    }
    SetCoverState* new_s =
        new SetCoverState(node, buffer_.Set().MaxNbElements());
    new_s->Set().Union(buffer_.Set());
    new_s->UpdateSP(path_value);
    return existing_states[s] = new_s;
  }
};

}  // namespace MDD

#endif /* SRC_DYNAMICPROG_SETCOVER */
