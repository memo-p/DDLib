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

#include "MDDInfos.hpp"

namespace MDD {

void printInfo(MDD &mdd, std::ostream &ss) {
  int nbNodes = 0;
  int numNodelvlSum = 0;
  int nbArc = 0;
  for (int i = 0; i <= mdd.getSize(); i++) {
    Node *n = mdd.nodes_[i].First();
    numNodelvlSum += mdd.nodes_[i].Size();
    while (n != nullptr) {
      Arc *e = n->Arcs();
      nbNodes++;
      while (e != nullptr) {
        nbArc++;
        e = e->next_;
      }
      n = n->next_;
    }
  }
  ss << "nodes : \t" << nbNodes << " " << numNodelvlSum << "\n";
  ss << "edges : \t" << nbArc << "\n";
}

void diff(MDD &mdd, MDD &mdd2, std::ostream &ss) {
  Node *n1;
  Node *n2;
  Arc *e1;
  Arc *e2;
  std::queue<Node *> qu1;
  std::queue<Node *> qu2;
  qu1.push(mdd.Root());
  qu2.push(mdd2.Root());

  while (!qu1.empty() && !qu2.empty()) {
    n1 = qu1.front();
    qu1.pop();
    n2 = qu2.front();
    qu2.pop();
    n1->visited_ = false;
    n2->visited_ = false;
    e1 = n1->Arcs();
    e2 = n2->Arcs();
    while (e1 != NULL && e2 != NULL) {
      if (e1->value_ != e2->value_) {
        ss << "les deux MDD ne sont pas equivalents\n";
        return;
      }
      if (!e1->end_->visited_) {
        e1->end_->visited_ = true;
        qu1.push(e1->end_);
      }
      if (!e2->end_->visited_) {
        e2->end_->visited_ = true;
        qu2.push(e2->end_);
      }
      e1 = e1->next_;
      e2 = e2->next_;
    }
  }

  if (!qu1.empty()) {
    ss << "mdd1 a des noeud en plus\n";
    return;
  }
  if (!qu2.empty()) {
    ss << "mdd1 a des noeud en plus\n";
    return;
  }
  ss << "les deux MDD sont equivalents\n";
  return;
}

}  // namespace MDD
