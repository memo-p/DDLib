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

#ifndef SRC_PROBA_CONFIDENCE
#define SRC_PROBA_CONFIDENCE

#include <stdio.h>

#include <iostream>
#include <queue>

#include "Core/MDD.hpp"
#include "Operations/Search.hpp"
#include "Proba/probdf.hpp"

namespace MDD {

/**
 * Class that count the number of tuples inside an MDD.
 **/
class ConfidenceDFS : public DepthFirstSearch {
 private:
  std::vector<double> nb_outgoing_paths_;
  PDF &pdf_;

 public:
  ConfidenceDFS(MDD &mdd, PDF &pdf)
      : DepthFirstSearch(mdd), nb_outgoing_paths_(mdd.node_id_, 0), pdf_(pdf) {
    nb_outgoing_paths_[mdd.Root()->UID()] = 1.;
  }

  void OnCloseArc(Arc *a) override {
    nb_outgoing_paths_[a->Start()->UID()] +=
        nb_outgoing_paths_[a->End()->UID()] *
        pdf_.Probability(a->Start()->VarIndex(), a->Value());
  }

  /**
   * Return the number of tuples processed.
   **/
  double GetConfidence() { return nb_outgoing_paths_[GetMDD().Final()->UID()]; }
};

/**
 * Return the number of tuples stored in an MDD.
 **/
inline double GetConfidence(MDD &mdd, PDF &pdf) {
  ConfidenceDFS ct_dfs(mdd, pdf);
  ct_dfs.Run();
  return ct_dfs.GetConfidence();
}

}  // namespace MDD

#endif /* SRC_PROBA_CONFIDENCE */
