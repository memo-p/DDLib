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

#ifndef SRC_UTILS_MDDINFOS
#define SRC_UTILS_MDDINFOS

#include <InfInt.h>
#include <stdio.h>

#include <iostream>
#include <queue>

#include "Core/MDD.hpp"
#include "Operations/Search.hpp"

namespace MDD {

void printInfo(MDD &mdd, std::ostream &ss = std::cout);

void diff(MDD &mdd, MDD &mdd2, std::ostream &ss = std::cout);

/**
 * Class that count the number of tuples inside an MDD.
 **/
class CountTuplesDFS : public DepthFirstSearch {
 private:
  std::vector<InfInt> nb_outgoing_paths_;

 public:
  CountTuplesDFS(MDD &mdd)
      : DepthFirstSearch(mdd), nb_outgoing_paths_(mdd.node_id_, 0) {
    nb_outgoing_paths_[mdd.Final()->UID()] = 1;
  }
  void OnCloseArc(Arc *a) override {
    nb_outgoing_paths_[a->Start()->UID()] +=
        nb_outgoing_paths_[a->End()->UID()];
  }
  /**
   * Return the number of tuples processed.
   **/
  InfInt GetNumberOfTuples() {
    return nb_outgoing_paths_[GetMDD().Root()->UID()];
  }
};

/**
 * Return the number of tuples stored in an MDD.
 **/
inline InfInt CountTuples(MDD &mdd) {
  CountTuplesDFS ct_dfs(mdd);
  ct_dfs.Run();
  return ct_dfs.GetNumberOfTuples();
}

class DrawDFS : public DepthFirstSearch {
 private:
  std::ostream &ss_;

 public:
  DrawDFS(MDD &mdd, std::ostream &ss) : DepthFirstSearch(mdd), ss_(ss) {}
  void OnStart() override { ss_ << "digraph G{\n"; }
  void OnStop() override { ss_ << "}\n"; }
  void OnOpenArc(Arc *a) override {
    ss_ << a->Start()->UID() << "->" << a->End()->UID()
        << "[ label = " << a->Value() << "]\n";
  }
  void OnOpenNode(Node *n) override { ss_ << n->UID() << " [color = red]\n";}
};

inline void Draw(MDD &mdd, std::ostream &ss = std::cout) {
  DrawDFS d(mdd, ss);
  d.Run();
}


}  // namespace MDD

#endif /* SRC_UTILS_MDDINFOS */
