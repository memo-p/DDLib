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

#include "Constructions/BuilderFromDynProg.hpp"
#include "DynamicProg/ApplyDP.hpp"
#include "DynamicProg/Constraints/AllDifferent.hpp"
#include "DynamicProg/DynSum.hpp"
#include "DynamicProg/MISP/mispbench.hpp"
#include "DynamicProg/SetCover/bench.hpp"
#include "Help/testHelper.hpp"
#include "Operations/Apply.hpp"
#include "Operations/ReduceDFSMap.hpp"
#include "Restricted/DPCreation.hpp"
#include "Relax/Partitioners/setPartitioner.hpp"
#include "Relax/Utils.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test SetCover relax creation") {
  std::vector<int> nb_variables = {9, 10, 12};
  std::vector<int> bandwidths = {3, 4, 5};
  std::vector<int> ones_per_rows_g = {2, 3};
  std::vector<int> depths = {0, 1, 3};
  std::vector<int> widths = {81,2,4,8,12};
  std::vector<int> seeds = {752190, 500, 0, 99999};
  std::vector<int> part_algos = {0, 1, 2, 3};
  // std::vector<int> nb_variables = {10};
  // std::vector<int> bandwidths = {3};
  // std::vector<int> ones_per_rows_g = {2};
  // std::vector<int> depths = {3};
  // std::vector<int> widths = {1};
  // std::vector<int> seeds = {0};
  // std::vector<int> part_algos = {3};
  for (int nb_vars : nb_variables) {
    for (int bandwidth : bandwidths) {
      for (int ones_per_rows : ones_per_rows_g) {
        for (int seed : seeds) {
          for (int max_depth : depths) {
            for (int max_width : widths) {
              for (int part_algo : part_algos) {
                int nb_values = 2;  // 0, 1
                SetCoverBench sc_bench(nb_vars, bandwidth, ones_per_rows, seed);
                sc_bench.BuildInstance();

                // Exact one
                SetCoverDP sc(sc_bench.RowsMembership(), sc_bench.Costs());
                MDDBuilderDynP dpc(&sc, nb_vars, nb_values);
                auto mdde = dpc.Build();
                Reduce rd(*mdde);
                checkMDD(mdde);

                // relax one
                SetCoverDP scRelax(sc_bench.RowsMembership(), sc_bench.Costs());
                Partitioner* partitioner = nullptr;
                DynamicProgRestrictedCreation dprc(nb_vars, nb_values, &scRelax,
                                              partitioner, max_width,
                                              max_depth);
                if (part_algo == 0) {
                  partitioner = new Partitioner();
                } else if (part_algo == 1) {
                  partitioner = new RandomPartitioner();
                } else if (part_algo == 2) {
                  partitioner = new MaxRankPartitioner<DynamicProgRestrictedCreation>(&dprc);
                } else if (part_algo == 3) {
                  partitioner = new MinRankPartitioner<DynamicProgRestrictedCreation>(&dprc);
                }
                dprc.SetPartitioner(partitioner);
                auto mdd = dprc.Build();

                // check inclusion.
                std::vector<int> values;
                for (size_t i = 0; i < nb_vars; i++) {
                  values.push_back(nb_values);
                }

                TableOfTuple table(nb_vars, values);
                FillTable(*mdd, &table);
                CHECK(table.NumberOfTuples() > 0);
                if(table.NumberOfTuples() == 0){
                  sc_bench.PrintInstance();
                  std::cout << "haha\n";
                }
                InfInt nb_tuples = CountTuples(*mdd);
                if (nb_tuples < std::numeric_limits<uint64_t>::max()) {
                  InfInt nbt = table.NumberOfTuples();
                  CHECK(nbt == nb_tuples);
                }
                InfInt nbtid = 0;
                for (std::vector<int> const& tuple : table.Tuples()) {
                  CHECK(mdde->contains(tuple));
                  nbtid++;
                }
                CHECK(nbtid == nb_tuples);

                ShortestPathBFS lp(*mdd, [&sc_bench](Arc* a) {
                  return sc_bench.Costs()[a->Start()->VarIndex()] * a->Value();
                });
                lp.Run();
                int lg_path = lp.GetShortestPath();

                ShortestPathBFS lp2(*mdde, [&sc_bench](Arc* a) {
                  return sc_bench.Costs()[a->Start()->VarIndex()] * a->Value();
                });
                lp2.Run();
                int lg_path_e = lp2.GetShortestPath();
                CHECK(lg_path_e <= lg_path);

                // sc_bench.PrintInstance();
                // std::cout << nb_tuples << "\n";
                // std::cout << CountTuples(*mdd) << "\n";
                // Draw(*mdde);
                // Draw(*mdd);
                // exit(0);
              }
            }
          }
        }
      }
    }
  }
}