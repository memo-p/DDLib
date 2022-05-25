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


#include <Algorithms/paths.hpp>
#include <Core/MDD.hpp>
#include <SetCover.hpp>
#include <Relax/Creation/DPRelaxCreation.hpp>
#include <Relax/Partitioners/StatePartitioner.hpp>
#include <cxxopts.hpp>
#include <random>
#include <unordered_map>

#include <SetCover/bench.hpp>

using namespace MDD;
void RelaxSetCover(int argc, char* argv[]) {
  cxxopts::Options options("Relax Set Cover", "run");

  options.add_options()("w,width-max", "Maximum number of nodes per layer",
                        cxxopts::value<int>());
  options.add_options()("d,depth-max", "Maximum number of nodes per layer",
                        cxxopts::value<int>());
  options.add_options()("n,nb-vars", "number of variable",
                        cxxopts::value<int>());
  options.add_options()("b,bandwidth", "Bandwidth",
                        cxxopts::value<int>());
  options.add_options()("o,ones-per-row", "Number of ones per row",
                        cxxopts::value<int>());
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
      "i,instance", "Draw the instance",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
  options.add_options()(
      "f,output-format",
      "output format, by defaut plain text. Can be changed to csv.",
      cxxopts::value<std::string>()->default_value("plain"));
  options.add_options()("h,help", "Print usage");
  auto result = options.parse(argc, argv);
  if (result.count("help") || result.count("width-max") == 0 ||
      result.count("depth-max") == 0 || result.count("nb-vars") == 0
      || result.count("bandwidth") == 0 || result.count("ones-per-row") == 0) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  int width = result["width-max"].as<int>();
  int depth = result["depth-max"].as<int>();
  int nb_vars = result["nb-vars"].as<int>();
  int bandwidth = result["bandwidth"].as<int>();
  int ones_per_row = result["ones-per-row"].as<int>();
  int seed = result["seed"].as<int>();
  std::string part_algo = result["partitioner"].as<std::string>();
  std::string formating = result["output-format"].as<std::string>();
  bool draw_mdd = result["mdd"].as<bool>();
  bool draw_instance = result["instance"].as<bool>();

  SetCoverBench sc_bench(nb_vars, bandwidth, ones_per_row, seed);
  sc_bench.BuildInstance();
  SetCoverResult res = sc_bench.BuildMDD(width, depth, part_algo);
  if (formating == "plain") {
    std::cout << "********* Solving Set Cover Problem ************" << std::endl;
    std::cout << "#variables    : " << nb_vars << std::endl;
    std::cout << "bandwidth    : " << bandwidth << std::endl;
    std::cout << "#ones_per_row    : " << ones_per_row << std::endl;
    std::cout << "Max-width : " << width << std::endl;
    std::cout << "Max-Depth : " << depth << std::endl;
    std::cout << "Partition : " << part_algo << std::endl;
    std::cout << "Seed      : " << seed << std::endl;
    std::cout << "Build(ms) : " << res.time_ << std::endl;
    std::cout << "sol count : " << res.nb_tuples_ << std::endl;
    std::cout << "Best sol  : " << res.lg_path_ << std::endl;

  } else if (formating == "csv") {
    std::cout << "" << nb_vars;
    std::cout << "," << bandwidth;
    std::cout << "," << ones_per_row;
    std::cout << "," << width;
    std::cout << "," << depth;
    std::cout << "," << part_algo;
    std::cout << "," << seed;
    std::cout << "," << res.time_;
    std::cout << "," << res.nb_tuples_;
    std::cout << "," << res.lg_path_ << std::endl;
  }
  if (draw_mdd) {
    Draw(*res.mdd_);
  }
  if (draw_instance) {
    sc_bench.PrintInstance();
  }
  
}

int main(int argc, char *argv[]) {;
  RelaxSetCover(argc, argv);
  return 0;
}