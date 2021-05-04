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

#ifndef SRC_DYNAMICPROG_SETCOVER_BENCH
#define SRC_DYNAMICPROG_SETCOVER_BENCH

#include <algorithm>  // std::shuffle
#include <random>
#include <unordered_map>

#include "Algorithms/paths.hpp"
#include "Core/MDD.hpp"
#include "DynamicProg/SetCover.hpp"
#include "Restricted/DPCreation.hpp"
#include "Relax/Partitioners/StatePartitioner.hpp"

namespace MDD {
class SetCoverBench;

struct SetCoverResult {
  SetCoverBench& bench_;
  int width_;
  int depth_;
  std::string part_algo_;
  InfInt nb_tuples_;
  int lg_path_;
  int64_t time_;
  MDD* mdd_;
  SetCoverResult(SetCoverBench& bench, int width, int depth,
                 std::string part_algo, InfInt nb_tuples, int lg_path,
                 int64_t time, MDD* mdd)
      : bench_(bench),
        width_(width),
        depth_(depth),
        part_algo_(part_algo),
        nb_tuples_(nb_tuples),
        lg_path_(lg_path),
        time_(time),
        mdd_(mdd) {}
};

class SetCoverBench {
  /**
   * Rows of the problem (sets)
   **/
  std::vector<BitSet> rows_;
  std::vector<std::vector<int>> rows_uncompressed_;
  /**
   * For each variable, the rows in which it appears.
   **/
  std::vector<BitSet> rows_membership_;
  /**
   * cost associated to each variable
   **/
  std::vector<int> costs_;
  int nb_variables_;
  int nb_rows_;
  int bandwidth_;
  int nb_act_per_band_;
  int seed_;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution;

 public:
  SetCoverBench(int nb_variables, int bandwidth, int nb_act_per_band,
                int seed = 500)
      : nb_variables_(nb_variables),
        nb_rows_(nb_variables - bandwidth),
        bandwidth_(bandwidth),
        nb_act_per_band_(nb_act_per_band),
        seed_(seed),
        generator(seed),
        distribution(0.0, 1.0) {
    assert(nb_rows_ > 0);
  }
  /**
   * Number of variables
   **/
  int NbVariables() { return nb_variables_; }
  /**
   * Number of rows
   **/
  int NbRows() { return nb_rows_; }
  /**
   * bandwidth
   **/
  int Bandwidth() { return bandwidth_; }
  /**
   * Number of ones per row
   **/
  int NbOnesPerRow() { return nb_act_per_band_; }
  int Seed() { return seed_; }

  /**
   * Return a number between 0 and 1.
   **/
  double Random() { return distribution(generator); }
  int RandomCost() { return (int)(10 * Random()) + 1; }

  /**
   * Return the encoding of the rows.
   **/
  std::vector<BitSet> const& Rows() { return rows_; }
  /**
   * Return the encoding of the rows membership.
   **/
  std::vector<BitSet> const& RowsMembership() { return rows_membership_; }
  /**
   * Return the costs associated to each variable.
   **/
  std::vector<int> const& Costs() { return costs_; }

  void BuildInstance() {
    std::vector<int> bandwidth(bandwidth_);
    std::iota(std::begin(bandwidth), std::end(bandwidth), 0);
    rows_membership_.reserve(NbVariables());
    for (int i = 0; i < NbVariables(); i++) {
      rows_membership_.emplace_back(NbRows());
    }
    rows_.reserve(NbRows());
    for (int i = 0; i < NbRows(); i++) {
      rows_.emplace_back(NbVariables());
      rows_uncompressed_.emplace_back();
      std::shuffle(bandwidth.begin(), bandwidth.end(), generator);
      for (int j = 0; j < NbOnesPerRow(); j++) {
        int var = i + bandwidth[j];
        rows_.back().Add(var);
        rows_uncompressed_.back().push_back(var);
        rows_membership_[var].Add(i);
      }
    }
    for (int i = 0; i < NbVariables(); i++) {
      costs_.emplace_back(RandomCost());
    }
  }

  void PrintInstance() {
    std::cout << "Minimize ";
    for (int i = 0; i < nb_variables_; i++) {
      if (i != 0) {
        std::cout << " + ";
      }
      std::cout << costs_[i] << "x" << i;
    }
    std::cout << "\n";

    for(std::vector<int> &row_members : rows_uncompressed_) {
      std::sort(row_members.begin(),row_members.end());
      for (int i : row_members) {
        if (i != row_members[0]) {
          std::cout << " + ";
        }
        std::cout << "x" << i;
      }
      std::cout << ">= 1\n";
    }
  }

  SetCoverResult BuildMDD(int width, int depth, std::string part_algo) {
    // Partitioner selection
    Partitioner* partitioner = nullptr;

    SetCoverDP bench(rows_membership_, costs_);
    DynamicProgRestrictedCreation dprc(nb_variables_, 2, &bench, partitioner, width,
                                  depth);
    if (part_algo == "last") {
      partitioner = new Partitioner();
    } else if (part_algo == "random") {
      partitioner = new RandomPartitioner();
    } else if (part_algo == "max") {
      partitioner = new MaxRankPartitioner<DynamicProgRestrictedCreation>(&dprc);
    } else if (part_algo == "min") {
      partitioner = new MinRankPartitioner<DynamicProgRestrictedCreation>(&dprc);
    } else if (part_algo == "min-pack") {
      partitioner = new MinRankPackPartitioner<DynamicProgRestrictedCreation>(&dprc);
    }else {
      std::cout << "bad partition algorithm : " << part_algo << std::endl;
      exit(0);
    }
    dprc.SetPartitioner(partitioner);

    // build the MDD.
    auto mdd = dprc.Build();

    // extract number of tuples
    InfInt nb_tuples = CountTuples(*mdd);

    // extraction of the best solution

    ShortestPathBFS lp(*mdd, [this](Arc* a) {
      return this->costs_[a->Start()->VarIndex()] * a->Value();
    });
    lp.Run();
    int lg_path = lp.GetShortestPath();
    

    return SetCoverResult(*this, width, depth, part_algo, nb_tuples, lg_path,
                          dprc.elapsed_m_second(), mdd);
  }
};

}  // namespace MDD
#endif /* SRC_DYNAMICPROG_SETCOVER_BENCH */
