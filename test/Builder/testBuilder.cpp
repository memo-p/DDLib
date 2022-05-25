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

#define CATCH_CONFIG_MAIN
#include <DynSum.hpp>

#include "Algorithms/paths.hpp"
#include "Builders/Automaton.hpp"
#include "Builders/DynProg.hpp"
#include "Builders/Transitions.hpp"
#include "Builders/Table.hpp"
#include "Builders/Grid.hpp"
#include "DataStructures/TableBuilder.hpp"
#include "DataStructures/TableSort.hpp"
#include "Help/testHelper.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test MDD table creation") {
  TableOfTuple table(3, {2, 3, 2});
  std::vector<std::vector<int>> tuples = {
      {1, 1, 1}, {0, 0, 1}, {0, 1, 0}, {0, 0, 0}};
  table.addTuples(tuples);

  TableMDDBuilder bt(table);
  MDD::MDD* mdd = bt.Build();
  checkMDD(mdd);
  CHECK(mdd->getSize() == 3);
  CHECK(mdd->contains({0, 0, 0}));
  CHECK(mdd->contains({0, 1, 0}));
  CHECK(mdd->contains({0, 0, 1}));
  CHECK(mdd->contains({1, 1, 1}));
  CHECK(!mdd->contains({1, 1, 0}));
  CHECK(!mdd->contains({1, 0, 0}));
  CHECK(!mdd->contains({1, 0, 1}));
  CHECK(!mdd->contains({0, 1, 1}));
}

TEST_CASE("test MDD tablerandom creation") {
  for (size_t seed = 0; seed < 5; seed++) {
    TableOfTuple* tableptr = MakeRandomTable(5, {4, 4, 3, 2, 5}, 50, seed);
    TableOfTuple& table = *tableptr;

    TableMDDBuilder bt(table);
    MDD::MDD* mdd = bt.Build();
    checkMDD(mdd);
    CHECK(mdd->getSize() == 5);
    for (auto&& t : table.Tuples()) {
      CHECK(mdd->contains(t));
    }
  }
}

TEST_CASE("test MDD transition creation") {
  std::vector<std::vector<int>> tuples = {{0, 0, 1}, {0, 1, 2}, {1, 0, 3},
                                          {1, 1, 3}, {2, 2, 3}, {2, 1, 4},
                                          {3, 0, 5}, {3, 1, 5}, {4, 0, 5}};

  TransitionMDDBuilder bt(tuples, 3);
  MDD::MDD* mdd = bt.Build();
  checkMDD(mdd);
  CHECK(mdd->getSize() == 3);
  CHECK(mdd->contains({0, 0, 0}));
  CHECK(mdd->contains({0, 0, 1}));
  CHECK(mdd->contains({0, 1, 0}));
  CHECK(mdd->contains({0, 1, 1}));
  CHECK(mdd->contains({1, 2, 0}));
  CHECK(mdd->contains({1, 2, 1}));
  CHECK(mdd->contains({1, 1, 0}));
  CHECK(!mdd->contains({0, 2, 0}));
  CHECK(!mdd->contains({0, 2, 1}));
  CHECK(!mdd->contains({1, 1, 1}));
  CHECK(!mdd->contains({1, 1, 2}));
}

TEST_CASE("test MDD Grid creation") {
  GridMDDBuilder bt(4, 3);
  // example sum all paths < 3
  bt.addStartingTransition(0, 0);
  bt.addStartingTransition(1, 1);
  bt.addStartingTransition(2, 2);

  bt.addTransition(0, 0, 0);
  bt.addTransition(0, 1, 1);
  bt.addTransition(0, 2, 2);
  bt.addTransition(1, 0, 1);
  bt.addTransition(1, 1, 2);
  bt.addTransition(2, 0, 2);

  bt.addEndingTransition(0, 0);
  bt.addEndingTransition(0, 1);
  bt.addEndingTransition(0, 2);
  bt.addEndingTransition(1, 0);
  bt.addEndingTransition(1, 1);
  bt.addEndingTransition(2, 0);

  MDD::MDD* mdd = bt.Build();
  checkMDD(mdd);
  CHECK(mdd->getSize() == 4);
  CHECK(mdd->contains({0, 0, 0, 0}));
  CHECK(mdd->contains({0, 0, 0, 1}));
  CHECK(mdd->contains({0, 0, 0, 2}));
  CHECK(mdd->contains({0, 0, 1, 0}));
  CHECK(mdd->contains({0, 0, 1, 1}));
  CHECK(mdd->contains({0, 0, 2, 0}));
  CHECK(mdd->contains({0, 1, 0, 0}));
  CHECK(mdd->contains({0, 1, 0, 1}));
  CHECK(mdd->contains({0, 1, 1, 0}));
  CHECK(mdd->contains({0, 2, 0, 0}));
  CHECK(mdd->contains({1, 0, 0, 0}));
  CHECK(mdd->contains({1, 0, 0, 1}));
  CHECK(mdd->contains({1, 0, 1, 0}));
  CHECK(mdd->contains({1, 1, 0, 0}));
  CHECK(mdd->contains({2, 0, 0, 0}));

  CHECK(!mdd->contains({0, 0, 1, 2}));
  CHECK(!mdd->contains({0, 0, 2, 1}));
  CHECK(!mdd->contains({0, 0, 2, 2}));
  CHECK(!mdd->contains({0, 1, 0, 2}));
  CHECK(!mdd->contains({0, 2, 1, 2}));
  CHECK(!mdd->contains({0, 2, 2, 0}));
  CHECK(!mdd->contains({0, 2, 2, 1}));
  CHECK(!mdd->contains({0, 2, 2, 2}));
  CHECK(!mdd->contains({1, 2, 2, 1}));
  CHECK(!mdd->contains({1, 2, 2, 2}));
  CHECK(!mdd->contains({2, 0, 0, 1}));
  CHECK(!mdd->contains({2, 0, 0, 2}));
  CHECK(!mdd->contains({2, 0, 2, 1}));
  CHECK(!mdd->contains({2, 0, 2, 2}));
  CHECK(!mdd->contains({2, 2, 1, 2}));
  CHECK(!mdd->contains({2, 2, 2, 0}));
  CHECK(!mdd->contains({2, 2, 2, 2}));
}

TEST_CASE("test MDD Automaton creation") {
  std::vector<std::vector<int>> transitions = {{0, 0, 0}, {0, 1, 1}, {0, 2, 2},
                                               {1, 0, 1}, {1, 1, 2}, {2, 0, 2}};

  AutomatonMDDBuilder bt(transitions, 4, 0, {0, 1, 2});
  MDD::MDD* mdd = bt.Build();
  checkMDD(mdd);
  CHECK(mdd->getSize() == 4);
  CHECK(mdd->contains({0, 0, 0, 0}));
  CHECK(mdd->contains({0, 0, 0, 1}));
  CHECK(mdd->contains({0, 0, 0, 2}));
  CHECK(mdd->contains({0, 0, 1, 0}));
  CHECK(mdd->contains({0, 0, 1, 1}));
  CHECK(mdd->contains({0, 0, 2, 0}));
  CHECK(mdd->contains({0, 1, 0, 0}));
  CHECK(mdd->contains({0, 1, 0, 1}));
  CHECK(mdd->contains({0, 1, 1, 0}));
  CHECK(mdd->contains({0, 2, 0, 0}));
  CHECK(mdd->contains({1, 0, 0, 0}));
  CHECK(mdd->contains({1, 0, 0, 1}));
  CHECK(mdd->contains({1, 0, 1, 0}));
  CHECK(mdd->contains({1, 1, 0, 0}));
  CHECK(mdd->contains({2, 0, 0, 0}));

  CHECK(!mdd->contains({0, 0, 1, 2}));
  CHECK(!mdd->contains({0, 0, 2, 1}));
  CHECK(!mdd->contains({0, 0, 2, 2}));
  CHECK(!mdd->contains({0, 1, 0, 2}));
  CHECK(!mdd->contains({0, 2, 1, 2}));
  CHECK(!mdd->contains({0, 2, 2, 0}));
  CHECK(!mdd->contains({0, 2, 2, 1}));
  CHECK(!mdd->contains({0, 2, 2, 2}));
  CHECK(!mdd->contains({1, 2, 2, 1}));
  CHECK(!mdd->contains({1, 2, 2, 2}));
  CHECK(!mdd->contains({2, 0, 0, 1}));
  CHECK(!mdd->contains({2, 0, 0, 2}));
  CHECK(!mdd->contains({2, 0, 2, 1}));
  CHECK(!mdd->contains({2, 0, 2, 2}));
  CHECK(!mdd->contains({2, 2, 1, 2}));
  CHECK(!mdd->contains({2, 2, 2, 0}));
  CHECK(!mdd->contains({2, 2, 2, 2}));
}
