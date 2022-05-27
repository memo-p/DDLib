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

#ifndef SRC_OPERATIONS_APPLY
#define SRC_OPERATIONS_APPLY

#include <stdio.h>
#include "Core/MDD.hpp"
#include "Core/Triplet.hpp"
#include "Utils/MDDInfos.hpp"

#include "Operations/Reduce.hpp"

#include "Utils/ChronoP.hpp"

namespace MDD {

class Apply {
 public:
 /**
  * Left operand.
  **/
  MDD& mdd1_;
  /**
  * Right operand.
  **/
  MDD& mdd2_;
  /**
  * L is the current list of triplet to process.
  **/
  Triplet *L, *tmp_trip;
  /**
   * Resulting MDD
   **/
  MDD* mdd;
  Node* x;
  Node* x1;
  Node* x2;
  Arc* ex;
  Arc* ex1;
  Arc* ex2;
  std::vector<Arc*> index_mdd1_a;
  std::vector<Node*> index_mdd1_n;
  std::vector<Node*> index_mdd2;
  std::queue<int> index_mdd1_non_null;
  std::queue<int> index_mdd2_non_null;
  const unsigned int op_;
  unsigned int op_not_not_;
  unsigned int op_not_yes_;
  unsigned int op_yes_not_;
  unsigned int op_yes_yes_;
  int domain_size;
  ChronoP TimerReduce;

  static Node* fakeNode;

 public:
  Apply(MDD& mdd1, MDD& mdd2, unsigned int op)
      : mdd1_(mdd1),
        mdd2_(mdd2),
        L(nullptr),
        tmp_trip(nullptr),
        mdd(nullptr),
        x(nullptr),
        x1(nullptr),
        x2(nullptr),
        ex(nullptr),
        ex1(nullptr),
        ex2(nullptr),
        index_mdd1_a(mdd1_.node_id_, nullptr),
        index_mdd1_n(mdd1_.node_id_, nullptr),
        index_mdd2(mdd2_.node_id_,nullptr),
        op_(op),
        domain_size(mdd1_.maxDomSize_) {}

  MDD* Run();
  /**
   * Create an temporary node that represents the couple 
   * (@p ter1, @p ter2). Create an arc directed to this 
   * node. (Note that until the merge of temporary is done in
   * BuildLayer, the arc point to ter2).
   * 
   * Algorithm 8 : PhD Thesis GP
   **/
  void addArcAndNode(Node* n, int value, Node* ter1, Node* ter2);

  /**
   * This method find equivalent temporary nodes (couple n1, n2)
   * and and create a unique node to represent each of them.
   * 
   * Algorithm 9 : PhD Thesis GP
   **/
  void BuildLayer();
  /**
   * Iterate over the newly created nodes.
   * For each of them, that are couples (ni,nj),
   * with ni from the first MDD ad nj from the second,
   * For each value that belongs to at least one node,
   * if an arc need to be created, a call to addArcAndNode is made.
   * 
   * Algorithm 8 : PhD Thesis GP
   **/
  void BuildOutgoingArcs();
  /**
   * Iterate over the newly created nodes.
   * For each of them, that are couples (ni,nj),
   * with ni from the first MDD ad nj from the second,
   * For each value in [0,d],
   * if an arc need to be created, a call to addArcAndNode is made.
   * 
   * Algorithm 8 : PhD Thesis GP
   **/
  void BuildOutgoingArcsValueBased();

  /**
   * The creation of an arc labeled by 'a', given (ni,nj),
   * with ni from the first MDD ad nj from the second,
   * for a given operation is given by the
   * following flags.
   * 
   * Let op_cond1_cond2_where_ be any possible flag. 
   * - cond1 = yes means "If ni contains an arc label by a", not = \not yes.
   * - cond2 = yes means "If nj contains an arc label by a", not = \not yes.
   * - where = all means "For all the layer except the last one"
   * - where = last means "For the last layer"
   * 
   * See Table 5.1 : PhD Thesis GP
   **/
  const static unsigned int op_not_not_all_;
  const static unsigned int op_not_yes_all_;
  const static unsigned int op_yes_not_all_;
  const static unsigned int op_yes_yes_all_;
  const static unsigned int op_not_not_last_;
  const static unsigned int op_not_yes_last_;
  const static unsigned int op_yes_not_last_;
  const static unsigned int op_yes_yes_last_;

  /**
   * The different operations can be seen as the union of the different 
   * possible flags.
   *
   * See Table 5.1 and figure 5.9 : PhD Thesis GP
   **/
  const static unsigned int op_intersection;
  const static unsigned int op_union;
  const static unsigned int op_difference;
  const static unsigned int op_xor;
  const static unsigned int op_not_union;
  const static unsigned int op_not_intersection;
};

}  // namespace MDD


#endif /* SRC_OPERATIONS_APPLY */
