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
#ifndef SRC_RELAX_UTILS
#define SRC_RELAX_UTILS

#include <InfInt.h>
#include <stdio.h>

#include <iostream>
#include <queue>

#include "Core/MDD.hpp"
#include "Operations/Search.hpp"
#include "Relax/State.hpp"

namespace MDD {
class DrawDFSStates : public DepthFirstSearch {
 private:
  std::ostream &ss_;
  std::unordered_map<int, State *> &states_;

 public:
  DrawDFSStates(MDD &mdd, std::unordered_map<int, State *> &states, std::ostream &ss)
      : DepthFirstSearch(mdd), ss_(ss), states_(states) {}
  void OnStart() override { ss_ << "digraph G{\n"; }
  void OnStop() override { ss_ << "}\n"; }
  void OnOpenArc(Arc *a) override {
    ss_ << a->Start()->UID() << "->" << a->End()->UID()
        << "[ label = " << a->Value() << "]\n";
  }
  void OnOpenNode(Node *n) override {
    ss_ << n->UID() << " [label = " << states_[n->UID()]->to_string() << "]\n";
  }
};

inline void DrawStates(MDD &mdd, std::unordered_map<int, State *> &states, std::ostream &ss = std::cout) {
  DrawDFSStates d(mdd, states, ss);
  d.Run();
}

}  // namespace MDD

#endif /* SRC_RELAX_UTILS */
