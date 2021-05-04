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

#ifndef SRC_DYNAMICPROG_INDEPSET
#define SRC_DYNAMICPROG_INDEPSET

#include <stdio.h>

#include <set>
#include <unordered_map>

#include "Core/MDD.hpp"
#include "DynamicProg/DynP.hpp"
#include "Relax/State.hpp"

namespace MDD {

/**
 * Class representing the state for the MISP problem.
 **/
class IndepSetState : public State {
 private:
  IntState is_;
  SetState bs_;
  // Shortest path of a node,
  int sp_;

 public:
  IndepSetState(int is, uint64_t nb_elements, bool full = false)
      : is_(is), bs_(nb_elements, full), sp_(std::numeric_limits<int>::min()) {}

  std::string to_string() override {
    return is_.to_string() + "-" + bs_.to_string();
  };

  int Node() { return is_.Value(); }
  int SetNode(int new_node) { return is_ = new_node; }
  BitSet& Set() { return bs_; }

  // Shortest path of a node,
  int SP() const { return sp_; }
  void UpdateSP(int sp) { sp_ = std::max(sp_, sp); }

  int Rank() override { return SP(); }
};

/**
 * Dynamic programming Maximum independent set problem.
 *
 **/
class MISPDP : public DynamicProgram {
  std::unordered_map<std::string, IndepSetState*> existing_states;
  std::vector<BitSet> const& neighbors_;
  std::vector<int> costs_;
  IndepSetState buffer_;

 public:
  MISPDP(std::vector<BitSet> const& neighbors, std::vector<int> costs)
      : neighbors_(neighbors), costs_(costs), buffer_(0, neighbors.size()) {}

  MISPDP(std::vector<BitSet> const& neighbors)
      : MISPDP(neighbors, std::vector<int>(neighbors.size(), 1)) {}

  State* MakeInitialState() override {
    return new IndepSetState(0, neighbors_.size());
  }

  State* TransitionFunction(State* s, int value) override {
    IndepSetState* ss = static_cast<IndepSetState*>(s);
    const int var = ss->Node();
    BitSet& bs = ss->Set();
    if (value) {
      if (!bs.Contains(var)) {
        buffer_.Set().Clear();
        buffer_.Set().Union(bs);
        buffer_.Set().Union(neighbors_[var]);
        buffer_.Set().Add(var);
        return GetBufferState(var + 1, ss->SP() + costs_[var]);
      }
      return nullptr;
    } else {
      buffer_.Set().Clear();
      buffer_.Set().Union(bs);
      buffer_.Set().Add(var);
      return GetBufferState(var + 1, ss->SP());
    }
  }

  State* Merge(std::set<State*> states) override {
    buffer_.Set().Clear();
    assert(states.size() > 0);
    IndepSetState* ss1 = static_cast<IndepSetState*>(*states.begin());
    buffer_.Set().Union(ss1->Set());
    int shortest_path = std::numeric_limits<int>::min();
    for (auto&& s : states) {
      IndepSetState* si = static_cast<IndepSetState*>(s);
      buffer_.Set().Intersection(si->Set());
      shortest_path = std::max(shortest_path, si->SP());
    }
    return GetBufferState(ss1->Node(), shortest_path);
  }

  void DrawGraph(std::ostream& ss = std::cout) {
    ss << "graph G {" << std::endl;
    for (size_t i = 0; i < neighbors_.size(); i++) {
      auto& bs = neighbors_[i];
      for (size_t j = i + 1; j < bs.MaxNbElements(); j++) {
        if (bs.Contains(j)) {
          ss << std::to_string(i) << "--" << std::to_string(j) << std::endl;
        }
      }
    }
    for (size_t i = 0; i < costs_.size(); i++) {
      ss << std::to_string(i) << " [label=\"" << std::to_string(i) << "-"
         << std::to_string(costs_[i]) << "\"]" << std::endl;
    }

    ss << "}" << std::endl;
  }

 protected:
  IndepSetState* GetBufferState(int node, int path_value) {
    buffer_.SetNode(node);
    auto s = buffer_.to_string();
    if (existing_states[s]) {
      auto ss = existing_states[s];
      ss->UpdateSP(path_value);
      return ss;
    }
    IndepSetState* new_s =
        new IndepSetState(node, buffer_.Set().MaxNbElements());
    new_s->Set().Union(buffer_.Set());
    new_s->UpdateSP(path_value);
    return existing_states[s] = new_s;
  }
};

}  // namespace MDD

#endif /* SRC_DYNAMICPROG_INDEPSET */
