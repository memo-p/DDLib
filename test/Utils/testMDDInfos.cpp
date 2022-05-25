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

#include "Builders/Transitions.hpp"
#include "Core/MDD.hpp"
#include "DataStructures/TableBuilder.hpp"
#include "Help/testHelper.hpp"
#include "Operations/Search.hpp"
#include "Utils/MDDInfos.hpp"
#include "catch.hpp"

using namespace MDD;
TEST_CASE("test of the DFS") {
  class testDFS : public DepthFirstSearch {
    std::vector<int> node_open_order_;
    int open_id_;
    std::vector<int> node_close_order_;
    int close_id_;
    std::vector<int> arc_order_;
    int arc_id_;

   public:
    testDFS(MDD::MDD& mdd)
        : DepthFirstSearch(mdd),
          node_open_order_({0, 2, 4, 1, 3, 5}),
          open_id_(0),
          node_close_order_({1, 4, 2, 5, 3, 0}),
          close_id_(0),
          arc_order_({0, 0, 0, 1, 1, 1, 1, 0, 2}),
          arc_id_(0) {}

    void OnOpenNode(Node* n) override {
      CHECK(open_id_ < node_open_order_.size());
      CHECK(n->UID() == node_open_order_[open_id_++]);
    }
    void OnCloseNode(Node* n) override {
      CHECK(close_id_ < node_close_order_.size());
      CHECK(n->UID() == node_close_order_[close_id_++]);
    }
    void OnOpenArc(Arc* a) override {
      CHECK(arc_id_ < arc_order_.size());
      CHECK(a->Value() == arc_order_[arc_id_++]);
    }
  };
  std::vector<std::vector<int>> arcs = {{0, 0, 1}, {0, 1, 2}, {1, 0, 3},
                                        {1, 1, 3}, {2, 2, 3}, {2, 1, 4},
                                        {3, 0, 5}, {3, 1, 5}, {4, 0, 5}};

  TransitionMDDBuilder bt(arcs, 3);
  MDD::MDD* mdd = bt.Build();
  testDFS t(*mdd);
  t.Run();
}

TEST_CASE("test of the BFS") {
  class testBFS : public BreadthFirstSearch {
    std::vector<int> node_open_order_;
    int open_id_;
    std::vector<int> node_close_order_;
    int close_id_;
    std::vector<int> arc_order_;
    int arc_id_;

   public:
    testBFS(MDD::MDD& mdd)
        : BreadthFirstSearch(mdd),
          node_open_order_({0, 2, 3, 4, 5, 1}),
          open_id_(0),
          node_close_order_({0, 2, 3, 4, 5, 1}),
          close_id_(0),
          arc_order_({0, 1, 0, 1, 1, 2, 0, 1, 0}),
          arc_id_(0) {}

    void OnOpenNode(Node* n) override {
      CHECK(open_id_ < node_open_order_.size());
      CHECK(n->UID() == node_open_order_[open_id_++]);
    }
    void OnCloseNode(Node* n) override {
      CHECK(close_id_ < node_close_order_.size());
      CHECK(n->UID() == node_close_order_[close_id_++]);
    }
    void OnArc(Arc* a) override {
      CHECK(arc_id_ < arc_order_.size());
      CHECK(a->Value() == arc_order_[arc_id_++]);
    }
  };
  std::vector<std::vector<int>> arcs = {{0, 0, 1}, {0, 1, 2}, {1, 0, 3},
                                        {1, 1, 3}, {2, 2, 3}, {2, 1, 4},
                                        {3, 0, 5}, {3, 1, 5}, {4, 0, 5}};

  TransitionMDDBuilder bt(arcs, 3);
  MDD::MDD* mdd = bt.Build();
  testBFS t(*mdd);
  t.Run();
}

TEST_CASE("test of the number of solution") {
  int nb_vars = 5;
  std::vector<int> domains = {4, 4, 3, 2, 5};
  int nb_tuples_1 = 200;
  for (size_t seed = 0; seed < 15; seed++) {
    TableOfTuple* tableptr1 =
        MakeRandomTable(nb_vars, domains, nb_tuples_1, seed);
    TableOfTuple& table1 = *tableptr1;
    TableMDDBuilder bt1(table1);
    MDD::MDD* mdd1 = bt1.Build();
    checkMDD(mdd1);
    CHECK(CountTuples(*mdd1) == table1.NumberOfTuples());
  }
}
