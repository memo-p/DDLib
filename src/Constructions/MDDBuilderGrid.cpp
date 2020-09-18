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

#include "MDDBuilderGrid.hpp"

namespace MDD {

MDDBuilderGrid::MDDBuilderGrid(int size, int numState)
    : mdd_(new MDD(size)), numState_(numState), size_(size), nb_vals_(0) {
  mdd_->BuildRootAndFinalNodes();
  grid_.resize(size_ - 1);
  for (int i = 0; i < size_ - 1; ++i) {
    grid_[i].resize(numState_);
    for (int j = 0; j < numState_; j++) {
      grid_[i][j] = mdd_->createNode(i + 1);
    }
  }
}

void MDDBuilderGrid::addTransition(int start, int value, int end) {
  nb_vals_ = (nb_vals_ < value)? value : nb_vals_;
  for (int depth = 0; depth < size_ - 2; ++depth) {
    grid_[depth][start]->AddArc(value, grid_[(1 + depth)][end]);
  }
}
void MDDBuilderGrid::addLastTransition(int start, int value, int end) {
  nb_vals_ = (nb_vals_ < value)? value : nb_vals_;
  for (int depth = 0; depth < size_ - 2; ++depth) {
    grid_[depth][start]->AddArcLast(value, grid_[(1 + depth)][end]);
  }
}
void MDDBuilderGrid::addStartingTransition(int value, int end) {
  nb_vals_ = (nb_vals_ < value)? value : nb_vals_;
  mdd_->Root()->AddArc(value, grid_.front()[end]);
}
void MDDBuilderGrid::addEndingTransition(int start, int value) {
  nb_vals_ = (nb_vals_ < value)? value : nb_vals_;
  grid_.back()[start]->AddArc(value, mdd_->Final());
}
void MDDBuilderGrid::addEndingLastTransition(int start, int value) {
  nb_vals_ = (nb_vals_ < value)? value : nb_vals_;
  grid_.back()[start]->AddArcLast(value, mdd_->Final());
}

MDD* MDDBuilderGrid::Build() {
  mdd_->setDomSize(nb_vals_+1);
  deleteBadNode();
  Reduce r(*mdd_);
  return mdd_;
}

void MDDBuilderGrid::deleteBadNode() {
  Arc *e, *e_next;
  for (int i = 0; i < size_ - 1; ++i) {
    for (int j = 0; j < numState_; j++) {
      if (grid_[i][j]->nb_in_ == 0) {
        e = grid_[i][j]->Arcs();
        while (e != nullptr) {
          e_next = e->Next();
          grid_[i][j]->deleteArc(e);
          e = e_next;
        }
        mdd_->removeList(grid_[i][j]);
      }
    }
  }
}

}  // namespace MDD
