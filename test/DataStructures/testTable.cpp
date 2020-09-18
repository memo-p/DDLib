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

#include "DataStructures/TableBuilder.hpp"
#include "DataStructures/TableSort.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test table creation") {
  std::vector<int> domains = {2, 3, 2};
  TableOfTuple table(3, domains);
  std::vector<std::vector<int>> tuples = {
      {0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {1, 1, 1}};
  table.addTuples(tuples);

  CHECK(table.NumberOfTuples() == 4);
  CHECK(table.NumberOfVar() == 3);
  CHECK(table.getDomSize(0) == 2);
  CHECK(table.getDomSize(1) == 3);
  CHECK(table.getDomSize(2) == 2);
  CHECK(table.getValue(0, 0) == 0);
  CHECK(table.getValue(1, 0) == 0);
  CHECK(table.getValue(1, 2) == 1);
  CHECK(table.getValue(2, 1) == 1);
  CHECK(table.getValue(2, 2) == 0);
  CHECK(table.getValue(3, 0) == 1);
  CHECK(table.getValue(3, 1) == 1);
  CHECK(table.getValue(3, 2) == 1);
  CHECK(table.contains({0, 0, 0}));
  CHECK(table.contains({0, 1, 0}));
  CHECK(table.contains({0, 0, 1}));
  CHECK(table.contains({1, 1, 1}));
  CHECK(!table.contains({1, 1, 0}));
  CHECK(!table.contains({1, 0, 0}));
  CHECK(!table.contains({1, 0, 1}));
  CHECK(!table.contains({0, 1, 1}));
}

TEST_CASE("test tablerandom creation") {
  int nb_tuples = 50;
  int nb_vars = 5;
  std::vector<int> domains = {4, 4, 3, 2, 5};
  for (size_t seed = 0; seed < 5; seed++) {
    TableOfTuple* tableptr = MakeRandomTable(nb_vars, domains, nb_tuples, seed);
    TableOfTuple& table = *tableptr;
    CHECK(table.NumberOfTuples() <= nb_tuples);
    for (size_t i = 0; i < table.NumberOfTuples(); i++) {
      for (size_t j = 0; j < domains.size(); j++) {
        CHECK(table.getValue(i, j) < domains[j]);
      }
    }
  }
}

TEST_CASE("test table sort") {
  std::vector<int> domains = {2, 3, 2};
  TableOfTuple table(3, domains);
  std::vector<std::vector<int>> tuples = {{1, 0, 0}, {0, 2, 1}, {0, 2, 0},
                                          {2, 1, 0}, {2, 0, 1}, {0, 1, 1}};
  table.addTuples(tuples);

  TableSortCount tsc;
  int* order = tsc.sortedIndex(table);
  for (size_t i = 1; i < table.NumberOfTuples(); i++) {
    for (size_t j = 0; j < domains.size(); j++) {
      CHECK(table.getValue(order[i], j) >= table.getValue(order[i - 1], j));
      if (table.getValue(order[i], j) > table.getValue(order[i - 1], j)) {
        break;
      }
    }
  }
}

TEST_CASE("test tablerandom creation + sort") {
  int nb_tuples = 50;
  int nb_vars = 5;
  std::vector<int> domains = {4, 4, 3, 2, 5};
  for (size_t seed = 0; seed < 5; seed++) {
    TableOfTuple* tableptr = MakeRandomTable(nb_vars, domains, nb_tuples, seed);
    TableOfTuple& table = *tableptr;
    CHECK(table.NumberOfTuples() <= nb_tuples);
    int* order = CountSort(table);
    for (size_t i = 1; i < table.NumberOfTuples(); i++) {
      for (size_t j = 0; j < domains.size(); j++) {
        CHECK(table.getValue(order[i], j) >= table.getValue(order[i - 1], j));
        if (table.getValue(order[i], j) > table.getValue(order[i - 1], j)) {
          break;
        }
      }
    }
  }
}
