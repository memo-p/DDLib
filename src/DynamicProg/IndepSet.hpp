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

#ifndef SRC_DYNAMICPROG_DYNSUM
#define SRC_DYNAMICPROG_DYNSUM

#include <stdio.h>

#include <set>
#include <unordered_map>

#include "Core/MDD.hpp"
#include "DynamicProg/DynP.hpp"
#include "Relax/State.hpp"

namespace MDD {

/**
 * Dynamic programming sum
 *
 **/
class MISP : public DynamicProgram {
  class IndepSetState : public State {
   private:
    IntState is_;
    SetState bs_;

   public:
    IndepSetState(int is, int nb_elements, bool full = false)
        : is_(is), bs_(nb_elements, full) {}

    std::string to_string() override {
      return is_.to_string() + bs_.to_string();
    };

    int Node() { return is_.Value(); }
    int SetNode(int new_node) { return is_ = new_node; }
    BitSet& Set() { return bs_; }
  };

  std::unordered_map<std::string, IndepSetState*> existing_states;
  const std::vector<BitSet>& neighbors_;
  IndepSetState buffer_;

 public:
  MISP(std::vector<BitSet>& neighbors)
      : neighbors_(neighbors), buffer_(0, neighbors.size()) {}

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
        return GetBufferState(var+1);
      }
      return nullptr;
    } else {
      buffer_.Set().Clear();
      buffer_.Set().Union(bs);
      buffer_.Set().Add(var);
      return GetBufferState(var+1);
    }
  }

  State* Merge(std::set<State*> states) override {
    buffer_.Set().Clear();
    assert(states.size() > 0);
    IndepSetState* ss1 = static_cast<IndepSetState*>(*states.begin());
    buffer_.Set().Union(ss1->Set());
    for (auto&& s : states) {
      IndepSetState* si = static_cast<IndepSetState*>(s);
      buffer_.Set().Intersection(si->Set());
    }
    return GetBufferState(ss1->Node());
  }

  void DrawGraph(std::ostream& ss = std::cout) {
    ss << "graph G {" << std::endl;
    for (size_t i = 0; i < neighbors_.size(); i++) {
      auto& bs = neighbors_[i];
      for (int j = i + 1; j < bs.MaxNbElements(); j++) {
        if (bs.Contains(j)) {
          ss << std::to_string(i) << "--" << std::to_string(j) << std::endl;
        }
      }
    }
    ss << "}" << std::endl;
  }

 protected:
  IndepSetState* GetBufferState(int node) {
    buffer_.SetNode(node);
    auto s = buffer_.to_string();
    if (existing_states[s]) {
      return existing_states[s];
    }
    IndepSetState* new_s = new IndepSetState(node, buffer_.Set().MaxNbElements());
    new_s->Set().Union(buffer_.Set());
    return existing_states[s] = new_s;
  }
};

}  // namespace MDD

// /**
//      * Return the two nodes with the largest value
//      **/
// void Select(const DblList<Node>& layer,
//               std::vector<Node*>& to_merge) override {
//     to_merge.resize(2, nullptr);
//     Node* n = layer.First();
//     while (n) {
//       if (to_merge[0] == nullptr) {
//         to_merge[0] = n;
//       } else if (to_merge[1] == nullptr) {
//         to_merge[1] = n;
//         break;
//       }
//       n = n->Next();
//     }
//   }
//     }

#endif /* SRC_DYNAMICPROG_DYNSUM */
