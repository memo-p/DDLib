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

#include <iostream>


#include <Core/MDD.hpp>
#include <Relax/Creation/DPRelaxCreation.hpp>
#include <Relax/Partitioners/StatePartitioner.hpp>
#include <Partitioners/setPartitioner.hpp>
#include <cxxopts.hpp>
#include <random>
#include <unordered_map>

#include <AllDifferent.hpp>

using namespace MDD;

void AllDiffBench(int argc, char* argv[]) {
  cxxopts::Options options("Relax MISP", "run");

  options.add_options()("w,width-max", "Maximum number of nodes per layer",
                        cxxopts::value<int>());
  options.add_options()("d,depth-max", "Maximum number of nodes per layer",
                        cxxopts::value<int>());
  options.add_options()("nb-vars", "number of variables",
                        cxxopts::value<int>());
  options.add_options()("nb-values", "number of values", cxxopts::value<int>());
  options.add_options()("partitioner",
                        "Algorithm for selecting nodes to merge (last, random)",
                        cxxopts::value<std::string>()->default_value("last"));
  options.add_options()(
      "m,mdd", "Draw the MDD (dot/graphviz)",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
  options.add_options()(
      "f,output-format",
      "output format, but defaut plain text. Can be changed to csv.",
      cxxopts::value<std::string>()->default_value("plain"));
  options.add_options()("h,help", "Print usage");

  auto result = options.parse(argc, argv);
  if (result.count("help") || result.count("width-max") == 0 ||
      result.count("depth-max") == 0 || result.count("nb-vars") == 0 ||
      result.count("nb-values") == 0) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  int width = result["width-max"].as<int>();
  int depth = result["depth-max"].as<int>();
  int nb_vars = result["nb-vars"].as<int>();
  int nb_values = result["nb-values"].as<int>();
  std::string part_algo = result["partitioner"].as<std::string>();
  std::string formating = result["output-format"].as<std::string>();

  AllDifferentDP sdpR(nb_values);

  Partitioner* partitioner = nullptr;
  DynamicProgRelaxCreation dprc(nb_vars, nb_values, &sdpR, partitioner, width,
                                depth);
  if (part_algo == "last") {
    partitioner = new Partitioner();
  } else if (part_algo == "random") {
    partitioner = new RandomPartitioner();
  } else if (part_algo == "max") {
    partitioner = new MaxRankPartitioner<DynamicProgRelaxCreation>(&dprc);
  } else if (part_algo == "min") {
    partitioner = new MinRankPartitioner<DynamicProgRelaxCreation>(&dprc);
  } else if (part_algo == "min-pack") {
    partitioner = new MinRankPackPartitioner<DynamicProgRelaxCreation>(&dprc);
  } else if (part_algo == "kmeans") {
    partitioner = new SmallKMeansPartitioner<DynamicProgRelaxCreation>(&dprc);
  } else {
    std::cout << "bad partition algorithm : " << part_algo << std::endl;
    exit(0);
  }
  dprc.SetPartitioner(partitioner);
  auto mdd = dprc.Build();

  int64_t time = dprc.elapsed_m_second();
  // extract number of tuples
  InfInt nb_tuples = CountTuples(*mdd);
  bool draw_mdd = result["mdd"].as<bool>();
  
  if (draw_mdd) {
    Draw(*mdd);
  }
  if (formating == "plain") {
    std::cout << "********* Solving AllDiff Problem ************" << std::endl;
    std::cout << "#Vars    : " << nb_vars << std::endl;
    std::cout << "#Vals    : " << nb_values << std::endl;
    std::cout << "Max-width : " << width << std::endl;
    std::cout << "Max-Depth : " << depth << std::endl;
    std::cout << "Partition : " << part_algo << std::endl;
    std::cout << "Build(ms) : " << time << std::endl;
    std::cout << "sol count : " << nb_tuples << std::endl;

  } else if (formating == "csv") {
    std::cout << "" << nb_vars;
    std::cout << "," << nb_values;
    std::cout << "," << width;
    std::cout << "," << depth;
    std::cout << "," << part_algo;
    std::cout << "," << time;
    std::cout << "," << nb_tuples << std::endl;
  }
  
}

int main(int argc, char *argv[]) {
  AllDiffBench(argc, argv);
  return 0;
}