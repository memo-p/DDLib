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

#ifndef SRC_RELAX_CREATION_DPRELAXCREATION
#define SRC_RELAX_CREATION_DPRELAXCREATION

#include <stdio.h>

#include <unordered_map>

#include "Constructions/MDDBuilder.hpp"
#include "Core/MDD.hpp"
#include "Relax/Reduction/reduce.hpp"
#include "DynamicProg/DynP.hpp"
#include "Relax/State.hpp"
#include "Utils/MDDInfos.hpp"

namespace MDD {

/**
 * Generic class for the creation of dynamic programming problems.
 *
 * \TODO(USER) redefine the behavior of virtuals for particular problems.
 *
 **/
class DynamicProgRelaxCreation : public MDDBuilder {
 private:
  /**
   * Number of variables.
   **/
  int nb_vars_;
  /**
   * Number of possible values for each variables
   **/
  int domain_size_;
  /**
   * Initial state.
   **/
  State* root_state_;
  /**
   * Number of layers to unroll before the merge of two nodes.
   **/
  int depth_relax_;
  /**
   * Maximum number of nodes per layer of the MDD.
   **/
  int width_relax_;
  /**
   * Link between nodes and states.
   **/
  std::unordered_map<int, State*> states_;
  /**
   * Link between states and nodes.
   * One per layer.
   **/
  std::vector<std::unordered_map<State*, Node*>> node_4_state_;
  /**
   * MDD currently being built.
   **/
  MDD* mdd_;
  /**
   * StopWatch for the build.
   **/
  ChronoP chrono_build_;

  DblList<Node> to_del_;
  std::set<State *> state_set_;
  /**
   * The dynamic program to build.
   **/
  DynamicProgram *dp_;

  /**
   * Heuristic of partition for a layer.
   **/
  Partitioner *partitioner_;

 public:
  DynamicProgRelaxCreation(int nb_vars, int domain_size, 
                          DynamicProgram *dp, Partitioner *partitioner,
                           int width_relax = 32, int depth_relax = 0)
      : nb_vars_(nb_vars),
        domain_size_(domain_size),
        root_state_(dp->MakeInitialState()),
        depth_relax_(depth_relax),
        width_relax_(width_relax),
        node_4_state_(nb_vars + 1),
        mdd_(nullptr),
        dp_(dp),
        partitioner_(partitioner) {}

  virtual ~DynamicProgRelaxCreation() {}

  MDD* Build() override {
    chrono_build_.Start();
    std::vector<Node*> to_merge;
    mdd_ = new MDD(nb_vars_);
    mdd_->BuildRootAndFinalNodes();
    states_[mdd_->Root()->UID()] = root_state_;
    for (int depth = 0; depth < mdd_->Size(); depth += DepthRelax() + 1) {
      Node* n = mdd_->getNodeLvl(depth);
      while (n) {
        BuildSubTree(n, DepthRelax() + 1);
        n = n->Next();
      }
      RelaxReduce reducer(*mdd_, WidthRelax(), partitioner_, false);
      for (int i = 0; i < DepthRelax() + 1; i++) {
        const int local_depth = depth + 1 + i;
        if (i < DepthRelax() && local_depth < mdd_->Size()) {
          reducer.ReduceLayer(local_depth);
        } else if (local_depth >= mdd_->Size()) {
          break;
        } else {
          reducer.ReduceLayer(local_depth, false);
          rebuildStates(local_depth, reducer);
          reducer.CleanLayer(local_depth);
        }
      }
    }
    chrono_build_.Stop();
    return mdd_;
  }

  /**
   * Rebuild the state after the reduction.
   **/
  void rebuildStates(int layer, RelaxReduce& reducer) {
    Node* n = mdd_->getNodeLvl(layer);
    while (n) {
      auto s = reducer.NodesForNode(n);
      assert(s.size() > 0);
      if (s.size() == 1) {
        Node* n1 = *(s.begin());
        assert(states_[n1->UID()]);
        states_[n->UID()] = states_[n1->UID()];
      } else {
        state_set_.clear();
        for (Node * ni : s) {
          assert(states_[ni->UID()]);
          state_set_.insert(states_[ni->UID()]);
        }
        states_[n->UID()] = dp_->Merge(state_set_);
      }
      n = n->Next();
    }
  }

  /**
   * Build the possibles arcs and create the associated states.
   **/
  void BuildOutgoingArcs(Node* n) {
    const int layer = n->VarIndex();
    const int layer_p1 = layer + 1;
    if (n->Arcs() == nullptr && layer < mdd_->Size()) {
      for (int val = 0; val < domain_size_; val++) {
        State* s = dp_->TransitionFunction(states_[n->UID()], val);
        Node* end = BuildNodeForState(s, layer_p1);
        if (end) {
          n->AddArcLast(val, end);
        }
      }
    }
  }

  Node* BuildNodeForState(State* s, int layer) {
    Node* end = nullptr;
    if (s) {
      if (node_4_state_[layer][s]) {
        end = node_4_state_[layer][s];
      } else {
        end = (layer == mdd_->Size()) ? mdd_->Final() : mdd_->createNode(layer);
        node_4_state_[layer][s] = end;
      }
      states_[end->UID()] = s;
    }
    return end;
  }

  /**
   * Build the subtree emanating from a node.
   **/
  void BuildSubTree(Node* n, int depth) {
    if (depth) {
      BuildOutgoingArcs(n);
      Arc* a = n->Arcs();
      while (a) {
        BuildSubTree(a->End(), depth - 1);
        a = a->Next();
      }
    }
  }

  std::unordered_map<int, State*>& States() { return states_; }

  MDD* GetMDD() const { return mdd_; }

  int DepthRelax() const { return depth_relax_; }
  int WidthRelax() const { return width_relax_; }
  int64_t elapsed_m_second() { return chrono_build_.elapsed_m_second(); }

};  // namespace MDD

}  // namespace MDD

#endif /* SRC_RELAX_CREATION_DPRELAXCREATION */
