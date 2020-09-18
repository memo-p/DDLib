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

#include "Constructions/BuilderTransitions.hpp"
#include "Core/MDD.hpp"
#include "Help/testHelper.hpp"
#include "Operations/Reduce.hpp"
#include "Operations/ReduceDFSMap.hpp"
#include "catch.hpp"

using namespace MDD;
TEST_CASE("test of the DFS") {
  std::vector<std::vector<int>> arcs = {
      {0, 0, 1}, {0, 1, 2}, {1, 0, 3}, {1, 1, 3}, {2, 1, 4}, {2, 2, 6},
      {3, 0, 5}, {3, 1, 5}, {4, 0, 5}, {3, 0, 5}, {3, 1, 5},
  };

  MDDBuilderFromTransition bt(arcs, 3);
  MDD::MDD* mdd = bt.Build();
  SECTION("Using DFS map reduce") {
    ReduceDFSMap r_dfs(*mdd);
    r_dfs.Run();
  }
  SECTION("Using p-reduce") { Reduce r_p(*mdd); }
  checkMDD(mdd);
}
