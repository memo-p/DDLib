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

#include <algorithm> // for count
#include "Constructions/BuilderFromAutomaton.hpp"
namespace MDD {

const int MDDBuilderAutomaton::start_id = 0;
const int MDDBuilderAutomaton::value_id = 1;
const int MDDBuilderAutomaton::end_id = 2;

MDD *MDDBuilderAutomaton::Build() {
  ExtractNumberOfStates();
  MDDBuilderGrid bt(nb_vars_, nb_states_);
  for (auto &&t : transitions_) {
    if (t[start_id] == start_) {
      bt.addStartingTransition(t[value_id], t[end_id]);
    }
    bt.addTransition(t[start_id], t[value_id], t[end_id]);
    if (std::count(finals_.begin(), finals_.end(), t[end_id]))
    {
      bt.addEndingTransition(t[start_id], t[value_id]);
    }
  }
  return bt.Build();
}

void MDDBuilderAutomaton::ExtractNumberOfStates() {
  nb_states_ = 0;
  for (auto &&t : transitions_) {
    if (t[start_id] > nb_states_) {
      nb_states_ = t[start_id];
    }
    if (t[end_id] > nb_states_) {
      nb_states_ = t[end_id];
    }
  }
  nb_states_++;
}

}  // namespace MDD
