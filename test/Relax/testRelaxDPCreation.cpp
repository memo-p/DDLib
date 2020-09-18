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

#include <unordered_map>

#include "DynamicProg/DynSum.hpp"
#include "Help/testHelper.hpp"
#include "Relax/Creation/DPRelaxCreation.hpp"
#include "Relax/Utils.hpp"
#include "catch.hpp"

using namespace MDD;

TEST_CASE("test basic relax creation") {
  State* root = new BasicState<int>(0);
  int nb_vars = 3;
  int nb_values = 2;  // 0, 1
  int max_width = 2;
  int max_depth = 1;
  int max_DPSum = 2;
  SumDynProg sdp(max_DPSum);
  Partitioner p;
  DynamicProgRelaxCreation dprc(nb_vars, nb_values, &sdp, &p, max_width,
                                max_depth);
  auto mdd = dprc.Build();
  DrawStates(*mdd, dprc.States());
  CHECK(true);
}
