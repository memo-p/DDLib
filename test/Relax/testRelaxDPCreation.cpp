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

#include <unordered_map>
#include <AllDifferent.hpp>
#include <DynSum.hpp>
#include <SetCover/bench.hpp>
#include <MISP/bench.hpp>

#include "Builders/DynProg.hpp"
#include "DynamicProg/ApplyDP.hpp"
#include "Help/testHelper.hpp"
#include "Operations/Apply.hpp"
#include "Operations/ReduceDFSMap.hpp"
#include "Relax/Creation/DPRelaxCreation.hpp"
#include "Relax/Partitioners/setPartitioner.hpp"
#include "Relax/Utils.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test basic relax creation") {
  for (int depth = 0; depth < 3; depth++) {
    int nb_vars = 4;
    int nb_values = 2;  // 0, 1
    int max_width = 2;
    int max_depth = depth;
    int max_DPSum = 2;
    SumDynProg sdp(max_DPSum);
    Partitioner p;
    DynamicProgRelaxCreation dprc(nb_vars, nb_values, &sdp, &p, max_width,
                                  max_depth);
    auto mdd = dprc.Build();
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          for (int l = 0; l < 2; l++) {
            if (i + j + k + l <= max_DPSum) {
              CHECK(mdd->contains({i, j, k, l}));
            }
          }
        }
      }
    }
  }
}

TEST_CASE("test relax apply intersection rdm exact") {
  int nb_vars = 5;
  std::vector<int> domains = {4, 4, 3, 2, 5};
  int nb_tuples_1 = 200;
  int nb_tuples_2 = 200;
  std::vector<int> depths = {0, 1, 2, 3};
  for (int depth : depths) {
    for (size_t seed = 0; seed < 5; seed++) {
      TableOfTuple* tableptr1 =
          MakeRandomTable(nb_vars, domains, nb_tuples_1, seed);
      TableOfTuple& table1 = *tableptr1;
      TableMDDBuilder bt1(table1);
      MDD::MDD* mdd1 = bt1.Build();
      checkMDD(mdd1);

      TableOfTuple* tableptr2 =
          MakeRandomTable(nb_vars, domains, nb_tuples_2, seed);
      TableOfTuple& table2 = *tableptr2;
      TableMDDBuilder bt2(table2);
      MDD::MDD* mdd2 = bt2.Build();
      checkMDD(mdd1);

      MDD::MDD* mdd_inter =
          RelaxApplyDP(*mdd1, *mdd2, 5, Apply::op_intersection,
                       nb_tuples_1 * nb_tuples_2, depth);

      ReduceDFSMap r_dfs(*mdd_inter);
      r_dfs.Run();

      checkMDD(mdd_inter);
      int nb_tuples_result = 0;
      for (auto&& t : table1.Tuples()) {
        if (table2.contains(t)) {
          nb_tuples_result++;
          CHECK(mdd_inter->contains(t));
        } else {
          CHECK(!mdd_inter->contains(t));
        }
      }
      for (auto&& t : table2.Tuples()) {
        if (table1.contains(t)) {
          CHECK(mdd_inter->contains(t));
        } else {
          CHECK(!mdd_inter->contains(t));
        }
      }
      CHECK(CountTuples(*mdd_inter) == nb_tuples_result);
    }
  }
}

TEST_CASE("test relax apply intersection rdm relax") {
  int nb_vars = 5;
  std::vector<int> domains = {4, 4, 3, 2, 5};
  int nb_tuples_1 = 200;
  int nb_tuples_2 = 200;
  std::vector<int> widths = {8, 16, 32};
  std::vector<int> depths = {0, 1, 2, 3};
  for (int width : widths) {
    for (int depth : depths) {
      for (size_t seed = 0; seed < 5; seed++) {
        TableOfTuple* tableptr1 =
            MakeRandomTable(nb_vars, domains, nb_tuples_1, seed);
        TableOfTuple& table1 = *tableptr1;
        TableMDDBuilder bt1(table1);
        MDD::MDD* mdd1 = bt1.Build();
        checkMDD(mdd1);

        TableOfTuple* tableptr2 =
            MakeRandomTable(nb_vars, domains, nb_tuples_2, seed);
        TableOfTuple& table2 = *tableptr2;
        TableMDDBuilder bt2(table2);
        MDD::MDD* mdd2 = bt2.Build();
        checkMDD(mdd1);

        MDD::MDD* mdd_inter =
            RelaxApplyDP(*mdd1, *mdd2, 5, Apply::op_intersection, width, depth);

        ReduceDFSMap r_dfs(*mdd_inter);
        r_dfs.Run();

        checkMDD(mdd_inter);

        int nb_tuples_result = 0;
        for (auto&& t : table1.Tuples()) {
          if (table2.contains(t)) {
            nb_tuples_result++;
            CHECK(mdd_inter->contains(t));
          }
        }
        for (auto&& t : table2.Tuples()) {
          if (table1.contains(t)) {
            CHECK(mdd_inter->contains(t));
          }
        }
        CHECK(CountTuples(*mdd_inter) >= nb_tuples_result);
      }
    }
  }
}

TEST_CASE("test MISP relax creation") {
  std::vector<int> widths = {2, 4, 7, 10};
  std::vector<int> seeds = {752190, 500, 198164, 99999};

  for (int seed_id = 0; seed_id < seeds.size(); seed_id++) {
    for (int depth = 0; depth < 3; depth++) {
      for (int w_id = 0; w_id < widths.size(); w_id++) {
        for (int part_algo = 0; part_algo < 3; part_algo++) {
          int nb_vars = 9;
          int nb_values = 2;  // 0, 1
          int max_width = widths[w_id];
          int max_depth = depth;
          int seed = seeds[seed_id];
          MISPBench misp_bench(nb_vars, 0.3, seed);
          misp_bench.BuildInstance();

          // Exact one
          MISPDP misp(misp_bench.Neighbors(), misp_bench.Costs());
          DynProgMDDBuilder dpc(&misp, nb_vars, nb_values);
          auto mdde = dpc.Build();
          Reduce rd(*mdde);
          checkMDD(mdde);

          // relax one
          MISPDP mispRelax(misp_bench.Neighbors(), misp_bench.Costs());
          Partitioner* partitioner = nullptr;
          DynamicProgRelaxCreation dprc(nb_vars, nb_values, &mispRelax,
                                        partitioner, max_width, max_depth);
          if (part_algo == 0) {
            partitioner = new Partitioner();
          } else if (part_algo == 1) {
            partitioner = new RandomPartitioner();
          } else if (part_algo == 2) {
            partitioner = new MaxRankPartitioner<DynamicProgRelaxCreation>(&dprc);
          }
          dprc.SetPartitioner(partitioner);
          auto mdd = dprc.Build();

          // check inclusion.
          std::vector<int> values;
          for (size_t i = 0; i < nb_vars; i++) {
            values.push_back(nb_values);
          }

          TableOfTuple table(nb_vars, values);
          FillTable(*mdde, &table);
          CHECK(table.NumberOfTuples() > 0);
          for (std::vector<int> const& tuple : table.Tuples()) {
            CHECK(mdd->contains(tuple));
          }
        }
      }
    }
  }
}

TEST_CASE("test Alldiff DP relax creation") {
  std::vector<int> widths = {2, 4, 7, 10};
  for (size_t nb_vars = 4; nb_vars < 7; nb_vars++) {
    for (size_t nb_values = 4; nb_values < 7; nb_values++) {
      if (nb_values < nb_vars) {
        continue;
      }
      for (int depth = 0; depth < 3; depth++) {
        for (int w_id = 0; w_id < widths.size(); w_id++) {
          for (int part_algo = 0; part_algo < 3; part_algo++) {
            int max_width = widths[w_id];
            int max_depth = depth;
            // Exact one
            AllDifferentDP sdp(nb_values);
            DynProgMDDBuilder dpc(&sdp, nb_vars, nb_values);
            auto mdde = dpc.Build();
            Reduce rd(*mdde);
            checkMDD(mdde);

            // relax one
            AllDifferentDP sdpR(nb_values);
            Partitioner* partitioner = nullptr;
            DynamicProgRelaxCreation dprc(nb_vars, nb_values, &sdpR,
                                          partitioner, max_width, max_depth);
            if (part_algo == 0) {
              partitioner = new Partitioner();
            } else if (part_algo == 1) {
              partitioner = new RandomPartitioner();
            } else if (part_algo == 2) {
              partitioner = new SmallKMeansPartitioner<DynamicProgRelaxCreation>(&dprc);
            }
            dprc.SetPartitioner(partitioner);
            auto mdd = dprc.Build();

            // check inclusion.
            std::vector<int> values;
            for (size_t i = 0; i < nb_vars; i++) {
              values.push_back(nb_values);
            }

            TableOfTuple table(nb_vars, values);
            FillTable(*mdde, &table);
            CHECK(table.NumberOfTuples() > 0);
            for (std::vector<int> const& tuple : table.Tuples()) {
              CHECK(mdd->contains(tuple));
            }
          }
        }
      }
    }
  }
}