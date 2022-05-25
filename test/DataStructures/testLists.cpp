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

#include "Builders/Transitions.hpp"
#include "Builders/Table.hpp"
#include "Builders/Grid.hpp"
#include "DataStructures/TableBuilder.hpp"
#include "DataStructures/TableSort.hpp"
#include "catch.hpp"

using namespace MDD;

/**
 * Class used for testing purpose
 **/
class E : public DblListElement<E> {
 private:
  int value_;

 public:
  E(int v = 0) : value_(v) {}
  int Value() { return value_; }
};

TEST_CASE("creation and add next") {
  E e;
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == nullptr);

  E e2;
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);

  e.AddNextList(&e2);  // List is e <-> e2
  CHECK(e.Next() == &e2);
  CHECK(e.Prev() == nullptr);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == &e);

  E e3;
  CHECK(e3.Next() == nullptr);
  CHECK(e3.Prev() == nullptr);

  e.AddNextList(&e3);  // List is e <-> e3 <-> e2
  CHECK(e.Next() == &e3);
  CHECK(e.Prev() == nullptr);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == &e3);
  CHECK(e3.Next() == &e2);
  CHECK(e3.Prev() == &e);

  e3.RemoveList();  // List is e <-> e2
  CHECK(e.Next() == &e2);
  CHECK(e.Prev() == nullptr);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == &e);
  CHECK(e3.Next() == nullptr);
  CHECK(e3.Prev() == nullptr);
}

TEST_CASE("creation and add prev") {
  E e;
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == nullptr);

  E e2;
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);

  e.AddPrevList(&e2);  // List is e2 <-> e
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e2);
  CHECK(e2.Next() == &e);
  CHECK(e2.Prev() == nullptr);

  E e3;
  CHECK(e3.Next() == nullptr);
  CHECK(e3.Prev() == nullptr);

  e.AddPrevList(&e3);  // List is e2 <-> e3 <-> e
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e3);
  CHECK(e2.Next() == &e3);
  CHECK(e2.Prev() == nullptr);
  CHECK(e3.Next() == &e);
  CHECK(e3.Prev() == &e2);

  e3.RemoveList();  // List is e2 <-> e
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e2);
  CHECK(e2.Next() == &e);
  CHECK(e2.Prev() == nullptr);
  CHECK(e3.Next() == nullptr);
  CHECK(e3.Prev() == nullptr);
}

TEST_CASE("creation Of list") {
  DblList<E> l;
  CHECK(l.First() == nullptr);
  CHECK(l.Last() == nullptr);
  CHECK(l.Size() == 0);

  E e;
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == nullptr);

  l.AddFirst(&e);  // List e
  CHECK(l.First() == &e);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 1);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == nullptr);

  E e2;
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);

  l.AddFirst(&e2);  // List e2 <-> e
  CHECK(l.First() == &e2);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 2);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e2);
  CHECK(e2.Next() == &e);
  CHECK(e2.Prev() == nullptr);

  E e3;
  CHECK(e3.Next() == nullptr);
  CHECK(e3.Prev() == nullptr);

  l.AddFirst(&e3);  // List e3 <-> e2 <-> e
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 3);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e2);
  CHECK(e2.Next() == &e);
  CHECK(e2.Prev() == &e3);
  CHECK(e3.Next() == &e2);
  CHECK(e3.Prev() == nullptr);

  l.Remove(&e2);  // List e3 <-> e
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 2);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e3);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);
  CHECK(e3.Next() == &e);
  CHECK(e3.Prev() == nullptr);

  DblList<E> l2;
  CHECK(l2.First() == nullptr);
  CHECK(l2.Last() == nullptr);
  CHECK(l2.Size() == 0);

  l2.AddLast(&e2);
  CHECK(l2.First() == &e2);
  CHECK(l2.Last() == &e2);
  CHECK(l2.Size() == 1);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);

  l.Append(l2);  // List e3 <-> e <-> e2
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e2);
  CHECK(l.Size() == 3);
  CHECK(l2.First() == nullptr);
  CHECK(l2.Last() == nullptr);
  CHECK(l2.Size() == 0);
  CHECK(e.Next() == &e2);
  CHECK(e.Prev() == &e3);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == &e);
  CHECK(e3.Next() == &e);
  CHECK(e3.Prev() == nullptr);
}

TEST_CASE("creation Of ordered list") {
  DblOrderedList<E> l;
  CHECK(l.First() == nullptr);
  CHECK(l.Last() == nullptr);
  CHECK(l.Size() == 0);

  CHECK(l.Find(2) == nullptr);
  E e1(1);
  E e2(2);
  E e3(3);
  E e4(4);
  E e8(8);
  E e8_2(8);
  E e10(10);

  l.Add(&e3);
  CHECK(l.Find(2) == nullptr);
  CHECK(l.Find(3) == &e3);
  CHECK(l.Find(4) == nullptr);
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e3);
  CHECK(l.Size() == 1);

  l.Add(&e2);
  l.Add(&e4);
  l.Add(&e1);
  CHECK(l.Find(-1) == nullptr);
  CHECK(l.Find(1) == &e1);
  CHECK(l.Find(2) == &e2);
  CHECK(l.Find(3) == &e3);
  CHECK(l.Find(4) == &e4);
  CHECK(l.Find(5) == nullptr);
  CHECK(l.First() == &e1);
  CHECK(l.Last() == &e4);
  CHECK(l.Size() == 4);

  l.Remove(&e3);
  CHECK(l.Find(-1) == nullptr);
  CHECK(l.Find(1) == &e1);
  CHECK(l.Find(2) == &e2);
  CHECK(l.Find(3) == nullptr);
  CHECK(l.Find(4) == &e4);
  CHECK(l.Find(5) == nullptr);
  CHECK(l.First() == &e1);
  CHECK(l.Last() == &e4);
  CHECK(l.Size() == 3);

  l.Add(&e8);
  l.Add(&e10);
  l.Add(&e8_2);
  CHECK(l.Find(-1) == nullptr);
  CHECK(l.Find(1) == &e1);
  CHECK(l.Find(2) == &e2);
  CHECK(l.Find(3) == nullptr);
  CHECK(l.Find(4) == &e4);
  CHECK(l.Find(5) == nullptr);
  CHECK(l.Find(8) != nullptr);
  CHECK(l.Find(9) == nullptr);
  CHECK(l.Find(10) == &e10);
  CHECK(l.First() == &e1);
  CHECK(l.Last() == &e10);
  CHECK(l.Size() == 6);

  auto e = l.First();
  CHECK(e->Value() == 1);
  e=e->Next();
  CHECK(e->Value() == 2);
  e=e->Next();
  CHECK(e->Value() == 4);
  e=e->Next();
  CHECK(e->Value() == 8);
  e=e->Next();
  CHECK(e->Value() == 8);
  e=e->Next();
  CHECK(e->Value() == 10);
  e=e->Next();
  CHECK(e == nullptr);
}

TEST_CASE("Test AddAfter and AddBefore") {
  DblList<E> l;
  CHECK(l.First() == nullptr);
  CHECK(l.Last() == nullptr);
  CHECK(l.Size() == 0);

  E e;
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == nullptr);

  l.AddFirst(&e);  // List e
  CHECK(l.First() == &e);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 1);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == nullptr);

  E e2;
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);

  l.AddBefore(&e2, &e);  // List e2 <-> e
  CHECK(l.First() == &e2);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 2);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e2);
  CHECK(e2.Next() == &e);
  CHECK(e2.Prev() == nullptr);

  E e3;
  CHECK(e3.Next() == nullptr);
  CHECK(e3.Prev() == nullptr);

  l.AddBefore(&e3, &e2);  // List e3 <-> e2 <-> e
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 3);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e2);
  CHECK(e2.Next() == &e);
  CHECK(e2.Prev() == &e3);
  CHECK(e3.Next() == &e2);
  CHECK(e3.Prev() == nullptr);

  l.Remove(&e2);  // List e3 <-> e
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e);
  CHECK(l.Size() == 2);
  CHECK(e.Next() == nullptr);
  CHECK(e.Prev() == &e3);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == nullptr);
  CHECK(e3.Next() == &e);
  CHECK(e3.Prev() == nullptr);


  l.AddAfter(&e2, &e);  // List e3 <-> e <-> e2
  CHECK(l.First() == &e3);
  CHECK(l.Last() == &e2);
  CHECK(l.Size() == 3);
  CHECK(e.Next() == &e2);
  CHECK(e.Prev() == &e3);
  CHECK(e2.Next() == nullptr);
  CHECK(e2.Prev() == &e);
  CHECK(e3.Next() == &e);
  CHECK(e3.Prev() == nullptr);
}