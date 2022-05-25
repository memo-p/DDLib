/*
 * MIT License
 *
 * Copyright (c) 2020 Guillaume Perez
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

#ifndef SRC_CONSTRUCTIONS_BUILDERFROMDYNPROG
#define SRC_CONSTRUCTIONS_BUILDERFROMDYNPROG

#include <stdio.h>

#include "Constructions/MDDBuilder.hpp"
#include "Constructions/MDDBuilderGrid.hpp"
#include "Core/MDD.hpp"
#include "DynamicProg/DynP.hpp"
#include "Operations/Reduce.hpp"

namespace MDD {

/**
 * Class used to build an MDD from a dynamic program
 **/
class MDDBuilderDynP : public MDDBuilder {
 public:
  MDDBuilderDynP(DynamicProgram *dp, int nb_vars, int domain_size)
      : dp_(dp),
        nb_vars_(nb_vars),
        domain_size_(domain_size),
        node_4_state_(nb_vars + 1) {}

  MDD *Build();
  void BuildOutgoingArcs(Node *n);
  Node *BuildNodeForState(State *s, int layer);

  DynamicProgram &DP() { return *dp_; }
  std::unordered_map<int, State *> &States() { return states_; }
  int64_t elapsed_m_second() { return chrono_build_.elapsed_m_second(); }

 private:
  /**
   * The dynamic program to build.
   **/
  DynamicProgram *dp_;
  /**
   * Number of variables.
   **/
  int nb_vars_;
  /**
   * Number of possible values for each variables
   **/
  int domain_size_;
  /**
   * Link between nodes and states.
   **/
  std::unordered_map<int, State *> states_;
  /**
   * Link between states and nodes.
   * One per layer.
   **/
  std::vector<std::unordered_map<State *, Node *>> node_4_state_;
  /**
   * MDD currently being built.
   **/
  MDD *mdd_;
  /**
   * StopWatch for the build.
   **/
  ChronoP chrono_build_;
};

}  // namespace MDD

#endif /* SRC_CONSTRUCTIONS_BUILDERFROMDYNPROG */
