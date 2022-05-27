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

#ifndef SRC_DATASTRUCTURES_TABLEBUILDER
#define SRC_DATASTRUCTURES_TABLEBUILDER

#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */

#include <fstream>
#include <stack>

#include "Core/MDD.hpp"
#include "Table.hpp"

namespace MDD {

/**
 * Build a table with at most @arg nb_tuples tuples.
 **/
TableOfTuple* MakeRandomTable(int numberOfVar, std::vector<int> const& domSize_,
                              int nb_tuples, int seed = 0);

void FillTableFromFile(TableOfTuple* tuples, char* filename, int arity);

/**
 * remplit la table avec les tuples du MDD
 */
void FillTable(MDD& mdd, TableOfTuple* tuples);

}  // namespace MDD

#endif /* SRC_DATASTRUCTURES_TABLEBUILDER */
