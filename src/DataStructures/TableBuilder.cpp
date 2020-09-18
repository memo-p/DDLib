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

#include "TableBuilder.hpp"

namespace MDD {

TableOfTuple* MakeRandomTable(int numberOfVar, std::vector<int> const& domSize_,
                              int nb_tuples, int seed) {
  TableOfTuple* table = new TableOfTuple(numberOfVar, domSize_);

  srand(seed);
  std::vector<int> tuple;
  tuple.resize(numberOfVar);
  for (int i = 0; i < nb_tuples; i++) {
    for (int j = 0; j < numberOfVar; j++) {
      tuple[j] = rand() % domSize_[j];
    }
    if (!table->contains(tuple)) {
      table->addTuple(tuple);
    }
  }

  return table;
}

void FillTableFromFile(TableOfTuple* tuples, char* filename, int arity) {
  std::ifstream* file = new std::ifstream(filename);
  std::vector<int> tuple;
  char* number = new char[256];
  while (file->good()) {
    tuple.clear();
    for (int i = 0; i < arity - 1; i++) {
      file->get(number, 256, ' ');
      file->get();
      tuple.push_back(atoi(number));
    }
    file->get(number, 256, '\n');
    file->get();
    tuple.push_back(atoi(number));
    tuples->addTuple(tuple);
  }

  file->close();
}

void FillTable(MDD& mdd, TableOfTuple* tuples) {
  std::vector<int> vals(tuples->NumberOfVar());
  std::stack<Arc*> stEdges;
  Arc* tmpEdge;
  stEdges.push(mdd.Root()->Arcs());
  while (!stEdges.empty()) {
    tmpEdge = stEdges.top();
    stEdges.pop();
    while (tmpEdge != nullptr) {
      vals[tmpEdge->start_->var_index_] = tmpEdge->value_;
      if (tmpEdge->end_->var_index_ == tuples->NumberOfVar()) {
        tuples->addTuple(vals);
        tmpEdge = tmpEdge->next_;
      } else {
        Arc* newEdge = tmpEdge->end_->Arcs();
        tmpEdge = tmpEdge->next_;
        stEdges.push(tmpEdge);
        tmpEdge = newEdge;
      }
    }
  }
}

}  // namespace MDD
