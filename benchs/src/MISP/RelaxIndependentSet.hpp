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

#ifndef BENCHS_SRC_RELAXINDEPENDENTSET
#define BENCHS_SRC_RELAXINDEPENDENTSET
#include <Algorithms/paths.hpp>
#include <Core/MDD.hpp>
#include <DynamicProg/IndepSet.hpp>
#include <Relax/Creation/DPRelaxCreation.hpp>
#include <Relax/Partitioners/StatePartitioner.hpp>
#include <cxxopts.hpp>
#include <random>
#include <unordered_map>

#include <DynamicProg/MISP/mispbench.hpp>

namespace MDD {
void RelaxIndependentSet(int argc, char* argv[]) {
  cxxopts::Options options("Relax MISP", "run");

  options.add_options()("w,width-max", "Maximum number of nodes per layer",
                        cxxopts::value<int>());
  options.add_options()("d,depth-max", "Maximum number of nodes per layer",
                        cxxopts::value<int>());
  options.add_options()("n,nb-nodes", "number of nodes for th MISP",
                        cxxopts::value<int>());
  options.add_options()("p,density", "Probability of  nodes being neighbors",
                        cxxopts::value<double>());
  options.add_options()("s,seed", "seed for random",
                        cxxopts::value<int>()->default_value("0"));
  options.add_options()(
      "partitioner",
      "Algorithm for selecting nodes to merge (last, random, max)",
      cxxopts::value<std::string>()->default_value("last"));
  options.add_options()(
      "m,mdd", "Draw the MDD (dot/graphviz)",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
  options.add_options()(
      "g,graph", "Draw the graph (dot/graphviz)",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
  options.add_options()(
      "f,output-format",
      "output format, but defaut plain text. Can be changed to csv.",
      cxxopts::value<std::string>()->default_value("plain"));
  options.add_options()("h,help", "Print usage");
  auto result = options.parse(argc, argv);
  if (result.count("help") || result.count("width-max") == 0 ||
      result.count("depth-max") == 0 || result.count("nb-nodes") == 0 ||
      result.count("density") == 0) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  int width = result["width-max"].as<int>();
  int depth = result["depth-max"].as<int>();
  int nb_nodes = result["nb-nodes"].as<int>();
  double density = result["density"].as<double>();
  int seed = result["seed"].as<int>();
  std::string part_algo = result["partitioner"].as<std::string>();
  std::string formating = result["output-format"].as<std::string>();
  // int draw_mdd = result["mdd"].as<bool>();
  // int draw_graph = result["graph"].as<bool>();

  MISPBench misp_bench(nb_nodes, density, seed);
  misp_bench.BuildInstance();
  MISPResult res = misp_bench.BuildMDD(width, depth, part_algo);
  if (formating == "plain") {
    std::cout << "********* Solving MISP Problem ************" << std::endl;
    std::cout << "#Nodes    : " << nb_nodes << std::endl;
    std::cout << "Density   : " << density << std::endl;
    std::cout << "Max-width : " << width << std::endl;
    std::cout << "Max-Depth : " << depth << std::endl;
    std::cout << "Partition : " << part_algo << std::endl;
    std::cout << "Seed      : " << seed << std::endl;
    std::cout << "Build(ms) : " << res.time_ << std::endl;
    std::cout << "sol count : " << res.nb_tuples_ << std::endl;
    std::cout << "Best sol  : " << res.lg_path_ << std::endl;

  } else if (formating == "csv") {
    std::cout << "" << nb_nodes;
    std::cout << "," << density;
    std::cout << "," << width;
    std::cout << "," << depth;
    std::cout << "," << part_algo;
    std::cout << "," << seed;
    std::cout << "," << res.time_;
    std::cout << "," << res.nb_tuples_;
    std::cout << "," << res.lg_path_ << std::endl;
  }
}

}  // namespace MDD

#endif /* BENCHS_SRC_RELAXINDEPENDENTSET */
