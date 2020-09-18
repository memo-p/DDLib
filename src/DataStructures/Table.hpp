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

#ifndef SRC_DATASTRUCTURES_TABLE
#define SRC_DATASTRUCTURES_TABLE
#include <sstream>
#include <string>
#include <vector>

namespace MDD {

class TableOfTuple {
  int numberOfVar_;
  std::vector<std::vector<int>> tuples_;

 public:
  std::vector<int> domSize_;

  TableOfTuple(int numberOfVar, std::vector<int> const &domSize)
      : numberOfVar_(numberOfVar), domSize_(domSize) {}

  void addTuple(std::vector<int> const &tuple) { tuples_.emplace_back(tuple); }
  void addTuples(std::vector<std::vector<int>> const &tuples) {
    for (auto &&t : tuples) addTuple(t);
  }

  int getValue(int line, int var) const { return tuples_[line][var]; }
  uint64_t NumberOfTuples() const { return tuples_.size(); }
  int NumberOfVar() const { return numberOfVar_; }
  int getDomSize(int var) const { return domSize_[var]; }
  std::vector<std::vector<int>> const &Tuples() { return tuples_; }

  bool contains(std::vector<int> const &tuple) const {
    bool equal;
    for (size_t i = 0; i < tuples_.size(); i++) {
      equal = true;
      for (size_t j = 0; j < tuples_[i].size(); j++) {
        if (tuples_[i][j] != tuple[j]) {
          equal = false;
          break;
        }
      }
      if (equal) {
        return true;
      }
    }
    return false;
  }

  std::string toString() const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < tuples_.size(); i++) {
      for (size_t j = 0; j < tuples_[i].size(); j++) {
        ss << tuples_[i][j] << " ";
      }
      ss << "\n";
    }
    ss << "]";
    return ss.str();
  }
};

}  // namespace MDD

#endif /* SRC_DATASTRUCTURES_TABLE */
