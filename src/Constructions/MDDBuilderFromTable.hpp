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

#ifndef SRC_CONSTRUCTIONS_MDDBUILDERFROMTABLE
#define SRC_CONSTRUCTIONS_MDDBUILDERFROMTABLE

#include <stdio.h>

#include "Constructions/MDDBuilder.hpp"
#include "Core/MDD.hpp"
#include "DataStructures/Table.hpp"
#include "DataStructures/TableSort.hpp"
#include "Operations/Reduce.hpp"

namespace MDD {

class MDDBuilderFromTable : public MDDBuilder {
 public:
  // the table must be sorted
  MDDBuilderFromTable(TableOfTuple &table, int *order);

  MDDBuilderFromTable(TableOfTuple &table)
      : MDDBuilderFromTable(table, CountSort(table)) {}

  MDD *Build();
  TableOfTuple &table_;
  int *order_;
};

}  // namespace MDD

#endif /* SRC_CONSTRUCTIONS_MDDBUILDERFROMTABLE */
