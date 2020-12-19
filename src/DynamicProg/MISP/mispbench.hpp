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

#ifndef BENCHS_SRC_MISP_MISPBENCH
#define BENCHS_SRC_MISP_MISPBENCH

#include "Algorithms/paths.hpp"
#include "Core/MDD.hpp"
#include "DynamicProg/IndepSet.hpp"
#include "Relax/Creation/DPRelaxCreation.hpp"
#include "Relax/Partitioners/StatePartitioner.hpp"
#include "random"
#include "unordered_map"

namespace MDD {
class MISPBench;

struct MISPResult {
  MISPBench& misp_;
  int width_;
  int depth_;
  std::string part_algo_;
  InfInt nb_tuples_;
  int lg_path_;
  int64_t time_;
  MDD* mdd_;
  MISPResult(MISPBench& misp, int width, int depth, std::string part_algo,
             InfInt nb_tuples, int lg_path, int64_t time, MDD* mdd)
      : misp_(misp),
        width_(width),
        depth_(depth),
        part_algo_(part_algo),
        nb_tuples_(nb_tuples),
        lg_path_(lg_path),
        time_(time),
        mdd_(mdd) {}
};

class MISPBench {
  std::vector<BitSet> neighbors_;
  std::vector<int> costs_;
  int nb_nodes_;
  double density_;
  int seed_;
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution;

 public:
  MISPBench(int nb_nodes, double density, int seed = 500)
      : nb_nodes_(nb_nodes),
        density_(density),
        seed_(seed),
        generator(seed),
        distribution(0.0, 1.0) {}
  /**
   * Number of nodes in the graph
   **/
  int NbNodes() { return nb_nodes_; }
  /**
   * density in the graph
   **/
  double Density() { return density_; }
  int Seed() { return seed_; }

  /**
   * Return a number between 0 and 1.
   **/
  double Random() { return distribution(generator); }
  int RandomCost() { return (int)(10 * Random()) + 1; }

  /**
   * Return the encoding of the graph as neighbors sets.
   **/
  std::vector<BitSet> const& Neighbors() { return neighbors_;}
  /**
   * Return the costs associated to each node.
   **/
  std::vector<int> const& Costs() { return costs_; }

  void BuildInstance() {
    neighbors_.reserve(NbNodes());
    for (int i = 0; i < NbNodes(); i++) {
      neighbors_.emplace_back(NbNodes());
      costs_.emplace_back(RandomCost());
    }

    for (int i = 0; i < NbNodes(); i++) {
      for (int j = i + 1; j < NbNodes(); j++) {
        if (Random() < Density()) {
          neighbors_[i].Add(j);
          neighbors_[j].Add(i);
        }
      }
    }
  }

  MISPResult BuildMDD(int width, int depth, std::string part_algo) {
    // Partitioner selection
    Partitioner* partitioner = nullptr;

    MISPDP misp(neighbors_, costs_);
    DynamicProgRelaxCreation dprc(nb_nodes_, 2, &misp, partitioner, width,
                                  depth);
    if (part_algo == "last") {
      partitioner = new Partitioner();
    } else if (part_algo == "random") {
      partitioner = new RandomPartitioner();
    } else if (part_algo == "max") {
      partitioner = new MaxRankPartitioner(&dprc);
    } else {
      std::cout << "bad partition algorithm : " << part_algo << std::endl;
      exit(0);
    }
    dprc.SetPartitioner(partitioner);

    // build the MDD.
    auto mdd = dprc.Build();

    // extract number of tuples
    InfInt nb_tuples = CountTuples(*mdd);

    // extraction of the best solution
    LongestPathBFS lp(*mdd, [this](Arc* a) {
      return this->costs_[a->Start()->VarIndex()] * a->Value();
    });
    lp.Run();
    int lg_path = lp.GetLongestPath();

    return MISPResult(*this, width, depth, part_algo, nb_tuples, lg_path,
                      dprc.elapsed_m_second(), mdd);
  }
};

}  // namespace MDD
#endif /* BENCHS_SRC_MISP_MISPBENCH */
