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

#ifndef TEST_TESTHELPER
#define TEST_TESTHELPER

#include <unordered_map>

#include "Core/MDD.hpp"
#include "Utils/MDDInfos.hpp"
#include "Operations/Search.hpp"
#include "catch.hpp"
#include "Constructions/BuilderFromAutomaton.hpp"
#include "Constructions/BuilderTransitions.hpp"
#include "Constructions/MDDBuilderFromTable.hpp"
#include "Constructions/MDDBuilderGrid.hpp"
#include "DataStructures/TableBuilder.hpp"
#include "DataStructures/TableSort.hpp"

namespace MDD {

/**
 * This function check that the arcs of a node are
 * ordered in incresing order w.r.t. their value.
 **/
inline bool AreArcsSorted(Node *n) {
  Arc *a = n->Arcs();
  while (a && a->Next()) {
    if (a->Value() > a->Next()->Value()) {
      return false;
    }
    a = a->Next();
  }
  return true;
}

/**
 * Checker that test that all arc are sorted.
 **/
class SortedChecker : public DepthFirstSearch {
 public:
  SortedChecker(MDD &mdd) : DepthFirstSearch(mdd) {}
  void OnOpenNode(Node *n) override { CHECK(AreArcsSorted(n)); }
};

/**
 * Checker that the MDD is reduced.
 **/
class ReduceChecker : public DepthFirstSearch {
 private:
  std::unordered_map<std::string, Node *> same_;

 public:
  ReduceChecker(MDD &mdd) : DepthFirstSearch(mdd) {}
  void OnOpenNode(Node *n) override {
    CHECK(same_[n->to_string()] == nullptr);
    same_[n->to_string()] = n;
  }
};

inline void checkMDD(MDD &mdd) {
  SortedChecker sc(mdd);
  sc.Run();
  ReduceChecker rc(mdd);
  rc.Run();
}
inline void checkMDD(MDD *mdd) { checkMDD(*mdd); }

}  // namespace MDD

#endif /* TEST_TESTHELPER */
