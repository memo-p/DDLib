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

#include "Apply.hpp"

namespace MDD {

void Apply::addArcAndNode(Node* n, int value, Node* ter1, Node* ter2) {
  if (n->VarIndex() == mdd->getSize() - 1) {
    n->AddArcLast(value, mdd->Final());
  } else {
    n->AddArcLast(value, ter2);
    if (index_mdd1_a[ter1->id_] == nullptr) {
      index_mdd1_a[ter1->id_] = n->lastArc();
      index_mdd1_n[ter1->id_] = ter1;
      index_mdd1_non_null.push(ter1->id_);
    } else {
      n->lastArc()->red_arc_ = index_mdd1_a[ter1->id_];
      index_mdd1_a[ter1->id_] = n->lastArc();
    }
  }
}

MDD* Apply::Run() {
  fakeNode->nb_in_++;
  mdd = new MDD(mdd1_.getSize());
  mdd->setDomSize(mdd1_.domSize_);

  mdd->BuildRootAndFinalNodes();

  op_not_not_ = op_not_not_all_;
  op_not_yes_ = op_not_yes_all_;
  op_yes_not_ = op_yes_not_all_;
  op_yes_yes_ = op_yes_yes_all_;

  L = new Triplet(mdd->Root(), mdd1_.Root(), mdd2_.Root());
  while (L != NULL) {
    if (L->x_->VarIndex() == mdd->getSize() - 1) {
      op_not_not_ = op_not_not_last_;
      op_not_yes_ = op_not_yes_last_;
      op_yes_not_ = op_yes_not_last_;
      op_yes_yes_ = op_yes_yes_last_;
    }
    if (op_not_not_ & op_) {
      BuildOutgoindArcsValueBased();
    } else {
      BuildOutgoindArcs();
    }
    BuildLayer();
  }

  TimerReduce.Start();
  Reduce r(*mdd);
  TimerReduce.Stop();
  return mdd;
}

void Apply::BuildLayer() {
  while (!index_mdd1_non_null.empty()) {
    ex = index_mdd1_a[index_mdd1_non_null.front()];
    x1 = index_mdd1_n[index_mdd1_non_null.front()];
    index_mdd1_a[index_mdd1_non_null.front()] = nullptr;
    index_mdd1_n[index_mdd1_non_null.front()] = nullptr;
    index_mdd1_non_null.pop();
    while (ex != NULL) {
      if (index_mdd2[ex->end_->id_] != nullptr) {
        ex->start_->updateArc(ex, index_mdd2[ex->end_->id_]);
      } else {
        index_mdd2[ex->end_->id_] = mdd->createNode(ex->start_->var_index_ + 1);
        index_mdd2_non_null.push(ex->end_->id_);
        tmp_trip = new Triplet(index_mdd2[ex->end_->id_], x1, ex->end_);
        tmp_trip->next_ = L;
        L = tmp_trip;
        ex->start_->updateArc(ex, index_mdd2[ex->end_->id_]);
      }
      ex1 = ex->red_arc_;
      ex->red_arc_ = nullptr;
      ex = ex1;
    }
    while (!index_mdd2_non_null.empty()) {
      index_mdd2[index_mdd2_non_null.front()] = nullptr;
      index_mdd2_non_null.pop();
    }
  }
}

void Apply::BuildOutgoindArcs() {
  while (L != NULL) {
    x = L->x_;
    x1 = L->x1_;
    x2 = L->x2_;
    tmp_trip = L;
    L = L->next_;
    delete tmp_trip;
    x1->visited_ = false;
    x2->newV_ = x2;
    ex1 = x1->Arcs();
    ex2 = x2->Arcs();
    while (ex1 != NULL) {
      while (ex2 != NULL && ex1->Value() > ex2->Value()) {
        if (op_not_yes_ & op_) {
          addArcAndNode(x, ex2->Value(), fakeNode, ex2->end_);
        }
        ex2 = ex2->next_;
      }
      if (ex2 != NULL && ex1->Value() == ex2->Value()) {
        if (op_yes_yes_ & op_) {
          addArcAndNode(x, ex1->Value(), ex1->end_, ex2->end_);
        }
        ex2 = ex2->next_;
      } else {
        if (op_yes_not_ & op_) {
          addArcAndNode(x, ex1->Value(), ex1->end_, fakeNode);
        }
      }
      ex1 = ex1->next_;
    }
    while (ex2 != nullptr) {
      if (op_not_yes_ & op_) {
        addArcAndNode(x, ex2->Value(), fakeNode, ex2->end_);
      }
      ex2 = ex2->next_;
    }
  }
}

void Apply::BuildOutgoindArcsValueBased() {
  while (L != NULL) {
    x = L->x_;
    x1 = L->x1_;
    x2 = L->x2_;
    tmp_trip = L;
    L = L->next_;
    delete tmp_trip;
    x1->visited_ = false;
    x2->newV_ = x2;
    ex1 = x1->Arcs();
    ex2 = x2->Arcs();
    for (int value = 0; value < domain_size; value++) {
      if (ex1 != NULL && ex1->Value() == value) {
        if (ex2 != NULL && ex2->Value() == value) {
          if (op_yes_yes_ & op_) {
            addArcAndNode(x, ex1->Value(), ex1->end_, ex2->end_);
          }
          ex2 = ex2->next_;
        } else {
          if (op_yes_not_ & op_) {
            addArcAndNode(x, ex1->Value(), ex1->end_, fakeNode);
          }
        }
        ex1 = ex1->next_;
      } else {
        if (ex2 != NULL && ex2->Value() == value) {
          if (op_not_yes_ & op_) {
            addArcAndNode(x, ex2->Value(), fakeNode, ex2->end_);
          }
          ex2 = ex2->next_;
        } else {
          if (op_not_not_ & op_) {
            addArcAndNode(x, ex1->Value(), fakeNode, fakeNode);
          }
        }
      }
    }
  }
}

const unsigned int Apply::op_not_not_all_ = 1u;
const unsigned int Apply::op_not_yes_all_ = 2u;
const unsigned int Apply::op_yes_not_all_ = 4u;
const unsigned int Apply::op_yes_yes_all_ = 8u;
const unsigned int Apply::op_not_not_last_ = 16u;
const unsigned int Apply::op_not_yes_last_ = 32u;
const unsigned int Apply::op_yes_not_last_ = 64u;
const unsigned int Apply::op_yes_yes_last_ = 128u;
const unsigned int Apply::op_intersection = op_yes_yes_all_ | op_yes_yes_last_;
const unsigned int Apply::op_union = op_intersection | op_not_yes_all_ |
                                     op_yes_not_all_ | op_not_yes_last_ |
                                     op_yes_not_last_;
const unsigned int Apply::op_difference =
    op_yes_not_all_ | op_yes_not_last_ | op_yes_yes_all_;
const unsigned int Apply::op_xor = op_yes_yes_all_ | op_not_yes_all_ |
                                   op_yes_not_all_ | op_not_yes_last_ |
                                   op_yes_not_last_;
const unsigned int Apply::op_not_union = op_not_not_all_ | op_not_not_last_ |
                                         op_not_yes_all_ | op_yes_not_all_ |
                                         op_yes_yes_all_;
const unsigned int Apply::op_not_intersection =
    op_not_union | op_not_yes_last_ | op_yes_not_last_;

Node* Apply::fakeNode = new Node(0, 0);

}  // namespace MDD
