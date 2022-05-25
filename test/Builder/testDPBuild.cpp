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
#include <set>

#include <AllDifferent.hpp>
#include <DynSum.hpp>
#include <MISP/bench.hpp>
#include <SetCover/bench.hpp>

#include "Algorithms/paths.hpp"
#include "Constructions/BuilderFromDynProg.hpp"
#include "DataStructures/TableBuilder.hpp"
#include "DataStructures/TableSort.hpp"
#include "Help/testHelper.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test basic DP creation") {
  int nb_vars = 4;
  int nb_values = 2;  // 0, 1
  int max_DPSum = 2;
  SumDynProg sdp(max_DPSum);
  MDDBuilderDynP dprc(&sdp, nb_vars, nb_values);
  auto mdd = dprc.Build();
  Reduce rd(*mdd);
  checkMDD(mdd);
  CHECK(LongestPath(*mdd) == 2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        for (int l = 0; l < 2; l++) {
          if (i + j + k + l <= 2) {
            CHECK(mdd->contains({i, j, k, l}));
          } else {
            CHECK(!mdd->contains({i, j, k, l}));
          }
        }
      }
    }
  }
}

TEST_CASE("test Alldiff DP creation") {
  for (size_t nb_vars = 4; nb_vars < 8; nb_vars++) {
    for (size_t nb_values = 4; nb_values < 8; nb_values++) {
      if (nb_values < nb_vars) {
        continue;
      }
      AllDifferentDP sdp(nb_values);
      MDDBuilderDynP dprc(&sdp, nb_vars, nb_values);
      auto mdd = dprc.Build();
      Reduce rd(*mdd);
      checkMDD(mdd);
      // check Alldiff.
      std::vector<int> values;
      for (size_t i = 0; i < nb_vars; i++) {
        values.push_back(nb_values);
      }
      TableOfTuple table(nb_vars, values);
      FillTable(*mdd, &table);
      CHECK(table.NumberOfTuples() > 0);
      for (std::vector<int> const& tuple : table.Tuples()) {
        std::set<int> s(tuple.begin(), tuple.end());
        CHECK(s.size() == nb_vars);
      }
    }
  }
}

TEST_CASE("test MISP creation") {
  std::vector<int> seeds = {752190, 500, 198164, 99999};

  for (size_t nb_vars = 4; nb_vars < 9; nb_vars++) {
    for (int seed_id = 0; seed_id < seeds.size(); seed_id++) {
      for (int part_algo = 0; part_algo < 3; part_algo++) {
        int nb_values = 2;  // 0, 1
        int seed = seeds[seed_id];
        MISPBench misp_bench(nb_vars, 0.3, seed);
        misp_bench.BuildInstance();

        // Exact one
        MISPDP misp(misp_bench.Neighbors(), misp_bench.Costs());
        MDDBuilderDynP dpc(&misp, nb_vars, nb_values);
        auto mdde = dpc.Build();
        Reduce rd(*mdde);
        checkMDD(mdde);

        // check inclusion.
        std::vector<int> values;
        for (size_t i = 0; i < nb_vars; i++) {
          values.push_back(nb_values);
        }

        // Check that all tuples are valids
        TableOfTuple table(nb_vars, values);
        FillTable(*mdde, &table);
        CHECK(table.NumberOfTuples() > 0);
        BitSet bs(nb_vars, false);
        for (std::vector<int> const& tuple : table.Tuples()) {
          bs.Clear();
          for (size_t i = 0; i < tuple.size(); i++) {
            if (tuple[i]) {
              bs.Union(misp_bench.Neighbors()[i]);
            }
          }
          for (size_t i = 0; i < tuple.size(); i++) {
            if (tuple[i]) {
              CHECK(!bs.Contains(i));
            }
          }
        }

        // Check that all solutions are created.
        MISPSolver mispsolver(misp_bench.Neighbors(), misp_bench.Costs());
        std::vector<std::vector<int>> mispsols = mispsolver.Solutions();
        CHECK(mispsols.size() > 0);
        for (std::vector<int> const& tuple : mispsols) {
            CHECK(mdde->contains(tuple));
        }
      }
    }
  }
}


TEST_CASE("test SetCover creation") {
  std::vector<int> nb_variables = {9, 10};
  std::vector<int> bandwidths = {3, 4};
  std::vector<int> ones_per_rows_g = {2, 3};
  std::vector<int> seeds = {752190, 500, 198164, 99999};
  std::vector<int> part_algos = {0, 1, 2};

  for (int nb_vars : nb_variables) {
    for (int bandwidth : bandwidths) {
      for (int ones_per_rows : ones_per_rows_g) {
        for (int seed : seeds) {
          for (int part_algo : part_algos) {
            int nb_values = 2;  // 0, 1
            int nb_rows = nb_vars - bandwidth;
            SetCoverBench sc_bench(nb_vars, bandwidth, ones_per_rows, seed);
            sc_bench.BuildInstance();

            // Exact one
            SetCoverDP sc(sc_bench.RowsMembership(), sc_bench.Costs());
            MDDBuilderDynP dpc(&sc, nb_vars, nb_values);
            auto mdde = dpc.Build();
            Reduce rd(*mdde);
            checkMDD(mdde);

            // check inclusion.
            std::vector<int> values;
            for (size_t i = 0; i < nb_vars; i++) {
              values.push_back(nb_values);
            }

            // Check that all tuples are valids
            TableOfTuple table(nb_vars, values);
            FillTable(*mdde, &table);
            CHECK(table.NumberOfTuples() > 0);

            BitSet bs(nb_rows, false);
            for (std::vector<int> const& tuple : table.Tuples()) {
              bs.Clear();
              for (size_t i = 0; i < tuple.size(); i++) {
                if (tuple[i]) {
                  bs.Union(sc_bench.RowsMembership()[i]);
                }
              }
              CHECK(bs.NbActives() == nb_rows);
                
            }

            // // Check that all solutions are created.
            // TODO
          }
        }
      }
    }
  }
}
