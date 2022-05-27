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

#include "Builders/DynProg.hpp"

#include <algorithm>  // for count
namespace MDD {

MDD* DynProgMDDBuilder::Build() {
  chrono_build_.Start();
  mdd_ = new MDD(nb_vars_);
  mdd_->BuildRootAndFinalNodes();
  mdd_->setDomSize(domain_size_);
  States()[mdd_->Root()->UID()] = DP().MakeInitialState();
  for (int depth = 0; depth < mdd_->Size(); depth++) {
    Node* n = mdd_->getNodeLvl(depth);
    while (n) {
      BuildOutgoingArcs(n);
      n = n->Next();
    }
  }
  return mdd_;
}

/**
 * Build the possibles arcs and create the associated states.
 **/
void DynProgMDDBuilder::BuildOutgoingArcs(Node* n) {
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

Node* DynProgMDDBuilder::BuildNodeForState(State* s, int layer) {
  Node* end = nullptr;
  if (s) {
    if (node_4_state_[layer][s]) {
      end = node_4_state_[layer][s];
    } else {
      end = (layer == mdd_->Size()) ? mdd_->Final() : mdd_->createNode(layer);
      node_4_state_[layer][s] = end;
    }
    States()[end->UID()] = s;
  }
  return end;
}
}  // namespace MDD
