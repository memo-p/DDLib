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

#define CATCH_CONFIG_MAIN

#include <unordered_map>

#include "Help/testHelper.hpp"
#include "Relax/Reduction/reduce.hpp"
#include "Constructions/BuilderFromAutomaton.hpp"
#include "Constructions/BuilderTransitions.hpp"
#include "Relax/Utils.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test basic relax reduce size 1") {
std::vector<std::vector<int>> transitions = {{0, 0, 1}, {0, 1, 2}, {1, 0, 3},
                                          {1, 1, 3}, {2, 2, 3}, {2, 1, 4},
                                          {3, 0, 5}, {3, 1, 5}, {4, 0, 5}};

  MDDBuilderFromTransition bt(transitions, 3);
  MDD::MDD* mdd = bt.Build();
  checkMDD(mdd);
  Partitioner p;
  RelaxReduce(*mdd, 1, &p);
  checkMDD(mdd);
  
  std::vector<std::vector<int>> tuples = 
  {{0,0,0},{0,0,1},{1,0,0},{1,0,1},
   {0,1,0},{0,1,1},{1,1,0},{1,1,1},
   {0,2,0},{0,2,1},{1,2,0},{1,2,1}};

  for(std::vector<int> const &t:tuples) {
    CHECK(mdd->contains(t));
  }
}



TEST_CASE("test relax reduce automaton") {
  std::vector<std::vector<int>> transitions = {
    {0, 1, 1}, {0, 2, 2}, {0, 3, 3},
    {1, 0, 0}, {1, 2, 2}, {1, 3, 3},
    {2, 0, 0}, {2, 1, 1}, {2, 3, 3},
    {3, 0, 0}, {3, 1, 1}, {3, 2, 2}};

  MDDBuilderAutomaton bt(transitions, 4, 0, {0, 1, 2, 3});
  MDD::MDD* mdd = bt.Build();
  checkMDD(mdd);
  // Draw(*mdd);

  Partitioner p;
  RelaxReduce(*mdd, 3, &p);
  // Draw(*mdd);
  checkMDD(mdd);
}


TEST_CASE("test relax reduce tablerandom creation") {
  for (size_t seed = 0; seed < 50; seed++) {
    TableOfTuple* tableptr = MakeRandomTable(5, {4, 4, 3, 2, 5}, 50, seed);
    TableOfTuple& table = *tableptr;

    MDDBuilderFromTable bt(table);
    MDD::MDD* mdd = bt.Build();
    checkMDD(mdd);
    SECTION("last part") {
      Partitioner p;
      RelaxReduce(*mdd, 3, &p);
    }
    SECTION("rdm part") {
      RandomPartitioner p;
      RelaxReduce(*mdd, 3, &p);
    }
    CHECK(mdd->getSize() == 5);
    for (auto&& t : table.Tuples()) {
      CHECK(mdd->contains(t));
    }
  }
}