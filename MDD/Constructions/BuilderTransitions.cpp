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

#include "Constructions/BuilderTransitions.hpp"
namespace MDD {

const int MDDBuilderFromTransition::start_id = 0;
const int MDDBuilderFromTransition::value_id = 1;
const int MDDBuilderFromTransition::end_id = 2;

MDD *MDDBuilderFromTransition::Build() {
  MDD *mdd = new MDD(nb_vars_);
  mdd->BuildRootAndFinalNodes();
  std::unordered_map<int, Node *> nodes;
  nodes[0] = mdd->Root();
  Node *start = nullptr;
  Node *end = nullptr;
  int v_max = 0;
  for (auto &&t : transitions_) {
    assert(nodes.find(t[start_id]) != nodes.end());
    start = nodes[t[start_id]];
    if (start->VarIndex() + 1 == nb_vars_) {
      end = mdd->Final();
    } else if (nodes.find(t[end_id]) != nodes.end()) {
      end = nodes[t[end_id]];
    } else {
      end = mdd->createNode(start->VarIndex() + 1);
      nodes[t[end_id]] = end;
    }
    start->AddArc(t[value_id], end);
    v_max = (v_max < t[value_id])? t[value_id]:v_max;
  }
  mdd->setDomSize(v_max+1);
  return mdd;
}

}  // namespace MDD
