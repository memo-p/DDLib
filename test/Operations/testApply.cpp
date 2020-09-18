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


#include "Constructions/MDDBuilderFromTable.hpp"
#include "DataStructures/TableBuilder.hpp"
#include "Operations/Apply.hpp"
#include "Help/testHelper.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test MDD intersection rdm") {
    int nb_vars = 5;
    std::vector<int> domains = {4, 4, 3, 2, 5};
    int nb_tuples_1 = 200;
    int nb_tuples_2 = 200;
  for (size_t seed = 0; seed < 5; seed++) {
    TableOfTuple* tableptr1 = MakeRandomTable(nb_vars, domains, nb_tuples_1, seed);
    TableOfTuple& table1 = *tableptr1;
    MDDBuilderFromTable bt1(table1);
    MDD::MDD* mdd1 = bt1.Build();
    checkMDD(mdd1);

    TableOfTuple* tableptr2 = MakeRandomTable(nb_vars, domains, nb_tuples_2, seed);
    TableOfTuple& table2 = *tableptr2;
    MDDBuilderFromTable bt2(table2);
    MDD::MDD* mdd2 = bt2.Build();
    checkMDD(mdd1);
    
    Apply inter(*mdd1, *mdd2, Apply::op_intersection);
    MDD::MDD* mdd_inter = inter.Run();
    checkMDD(mdd_inter);
    int nb_tuples_result = 0;
    for (auto&& t : table1.Tuples()) {
        if (table2.contains(t)) {
            nb_tuples_result++;
            CHECK(mdd_inter->contains(t));
        }else {
            CHECK(!mdd_inter->contains(t));
        }
    }
    for (auto&& t : table2.Tuples()) {
        if (table1.contains(t)) {
            CHECK(mdd_inter->contains(t));
        }else {
            CHECK(!mdd_inter->contains(t));
        }
    }
    CHECK(CountTuples(*mdd_inter) == nb_tuples_result);
  }
}

TEST_CASE("test MDD union rdm") {
    int nb_vars = 5;
    std::vector<int> domains = {4, 4, 3, 2, 5};
    int nb_tuples_1 = 200;
    int nb_tuples_2 = 200;
  for (size_t seed = 0; seed < 5; seed++) {
    TableOfTuple* tableptr1 = MakeRandomTable(nb_vars, domains, nb_tuples_1, seed);
    TableOfTuple& table1 = *tableptr1;
    MDDBuilderFromTable bt1(table1);
    MDD::MDD* mdd1 = bt1.Build();
    checkMDD(mdd1);

    TableOfTuple* tableptr2 = MakeRandomTable(nb_vars, domains, nb_tuples_2, seed);
    TableOfTuple& table2 = *tableptr2;
    MDDBuilderFromTable bt2(table2);
    MDD::MDD* mdd2 = bt2.Build();
    checkMDD(mdd1);
    
    Apply inter(*mdd1, *mdd2, Apply::op_union);
    MDD::MDD* mdd_inter = inter.Run();
    checkMDD(mdd_inter);
    int nb_tuples_result = table1.NumberOfTuples() + table2.NumberOfTuples();
    for (auto&& t : table1.Tuples()) {
        if (table2.contains(t)) {
            nb_tuples_result--;
        }
        CHECK(mdd_inter->contains(t));
    }
    for (auto&& t : table2.Tuples()) {
        CHECK(mdd_inter->contains(t));
    }
    CHECK(CountTuples(*mdd_inter) == nb_tuples_result);
  }
}