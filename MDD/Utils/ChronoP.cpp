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

#include "ChronoP.hpp"

namespace MDD {

void ChronoP::Start() { start = std::chrono::system_clock::now(); }
void ChronoP::Stop() { end = std::chrono::system_clock::now(); }
void ChronoP::Restart() { start = std::chrono::system_clock::now(); }

int64_t ChronoP::elapsed_second() {
  return std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
}
int64_t ChronoP::elapsed_m_second() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
      .count();
}
int64_t ChronoP::elapsed_u_second() {
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
      .count();
}

int64_t ChronoP::compute_granularity() {
  std::chrono::time_point<std::chrono::system_clock> tmp;
  std::chrono::time_point<std::chrono::system_clock> st =
      std::chrono::system_clock::now();
  while ((tmp = std::chrono::system_clock::now()) == st)
    ;
  return std::chrono::duration_cast<std::chrono::nanoseconds>(tmp - st).count();
}

}  // namespace MDD
