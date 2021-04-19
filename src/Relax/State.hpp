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

#ifndef SRC_RELAX_STATE
#define SRC_RELAX_STATE

#include <stdio.h>

#include <iterator>
#include <queue>
#include <sstream>

#include "Core/MDD.hpp"
#include "DataStructures/BitSets.hpp"
#include "DataStructures/SparseSet.hpp"
#include "Utils/ChronoP.hpp"

namespace MDD {

/**
 * Class state is used as basic type for state representation
 * for the creation of dynamic programming models.
 **/
class State {
 public:
  virtual ~State() {}
  /**
   * Unique string that represent the state.
   **/
  virtual std::string to_string() = 0;

  /**
   * Rank that can be used for some partitioning heuristic.
   **/
  virtual int Rank() { return 0; }
};

/**
 * Basic type state.
 **/
template <class T>
class BasicState : public State {
  T value_;

 public:
  BasicState(T value) : value_(value) {}

  void Set(T value) { value_ = value; }

  T Value() const { return value_; }
  std::string to_string() override { return std::to_string(value_); }

  operator T() const { return value_; }

  T operator++() { return ++value_; }
  T operator++(int) { return value_++; }

  T operator--() { return --value_; }
  T operator--(int) { return value_--; }

  T operator=(T v) { return value_ = v; }

  T operator+=(T v) { return value_ += v; }
  T operator-=(T v) { return value_ -= v; }
  T operator*=(T v) { return value_ *= v; }
  T operator/=(T v) { return value_ /= v; }

  T operator&=(T v) { return value_ &= v; }
  T operator|=(T v) { return value_ |= v; }

  T operator&(T v) { return value_ & v; }
  T operator|(T v) { return value_ | v; }

  T operator~() { return ~value_; }

  int Rank() override { return static_cast<int>(value_); }
};

class SmallBitsState : public BasicState<uint64_t> {
 public:
  SmallBitsState(uint64_t value) : BasicState<uint64_t>(value) {}

  int Rank() override { return static_cast<int>(NbOfBitSets(Value())); }
};

typedef BasicState<int> IntState;

/**
 * BitSet implementation of a state.
 **/
class SetState : public State, public BitSet {
 public:
  SetState(uint64_t nb_elements, bool full = false)
      : BitSet(nb_elements, full) {}
  std::string to_string() override {
    std::ostringstream oss;
    auto v = Words();
    if (!v.empty()) {
      std::copy(v.begin(), v.end() - 1,
                std::ostream_iterator<uint64_t>(oss, ":"));
      oss << v.back();
    }
    return oss.str();
  }

  int Rank() override { return static_cast<int>(NbActives()); }
};

}  // namespace MDD

#endif /* SRC_RELAX_STATE */
