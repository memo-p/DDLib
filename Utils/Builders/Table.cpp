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

#include "Table.hpp"
namespace MDD {

TableMDDBuilder::TableMDDBuilder(TableOfTuple& table, int* order)
    : table_(table), order_(order),order_local_(false) {}

MDD* TableMDDBuilder::Build() {
  MDD* mdd = new MDD(table_.NumberOfVar());
  mdd->setDomSize(table_.domSize_);
  mdd->BuildRootAndFinalNodes();
  Node* n;
  Arc* e;
  int val;
  for (size_t t = 0; t < table_.NumberOfTuples(); t++) {
    n = mdd->Root();
    for (int var = 0; var < table_.NumberOfVar(); var++) {
      val = table_.getValue(order_[t], var);
      e = n->lastArc();
      if (e == nullptr || e->Value() < val) {
        if (var + 1 < table_.NumberOfVar()) {
          n->AddArcLast(val, mdd->createNode(var + 1));
        } else {
          n->AddArcLast(val, mdd->Final());
        }
      }
      n = n->lastArc()->end_;
    }
  }
  Reduce r(*mdd);
  return mdd;
}

}  // namespace MDD
