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

#ifndef MDD_BUILDERS_TABLE
#define MDD_BUILDERS_TABLE

#include <stdio.h>

#include "Builders/base.hpp"
#include "Core/MDD.hpp"
#include "DataStructures/Table.hpp"
#include "DataStructures/TableSort.hpp"
#include "Operations/Reduce.hpp"

namespace MDD {

class TableMDDBuilder : public MDDBuilder {
 public:
  // the table must be sorted
  TableMDDBuilder(TableOfTuple &table, int *order);

  TableMDDBuilder(TableOfTuple &table)
      : TableMDDBuilder(table, CountSort(table)) {
    order_local_ = true;
  }

  ~TableMDDBuilder() {
    if (order_local_) {
      delete[] order_;
    }
    
  }

  MDD *Build();

 private:
  TableOfTuple &table_;
  int *order_;
  bool order_local_;
};

}  // namespace MDD

#endif /* MDD_BUILDERS_TABLE */
