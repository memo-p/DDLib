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

#include "BitSets.hpp"

namespace MDD {

void PrintBinary(uint64_t n, std::ostream &ss) {
  for (uint64_t i = sizeof(uint64_t) * 8 - 1; i > 0; i--) {
    ss << (((1l << i) & n) >> i) << " ";
  }
  ss << (n & 1) << " ";
}

uint64_t NextPowerOf2(uint64_t n) {
  unsigned i;
  --n;
  for (i = 1; i < sizeof n * __CHAR_BIT__; i <<= 1) n |= n >> i;
  return ++n;
}

bool BitSet::Contains(uint64_t k) const {
  return (OneBit(BitIndex(k)) & words_[WordIndex(k)]) > 0ul;
}

void BitSet::Add(uint64_t k) { words_[WordIndex(k)] |= OneBit(BitIndex(k)); }

void BitSet::Remove(uint64_t k) {
  words_[WordIndex(k)] = UnsetBit(words_[WordIndex(k)], BitIndex(k));
}

void BitSet::Restrict(uint64_t k) {
  Clear();
  Add(k);
}

void BitSet::Clear() {
  for (size_t i = 0; i < size_; ++i) {
    words_[i] = 0ul;
  }
}

void BitSet::Intersection(const BitSet &bs) {
  for (size_t i = 0; i < size_; ++i) {
    words_[i] &= bs.words_[i];
  }
}

void BitSet::Xor(const BitSet &bs){
  for (size_t i = 0; i < size_; ++i) {
    words_[i] ^= bs.words_[i];
  }
}

void BitSet::Union(const BitSet &bs) {
  for (size_t i = 0; i < size_; ++i) {
    words_[i] |= bs.words_[i];
  }
}

void BitSet::Not() {
  for (size_t i = 0; i < size_; ++i) {
    words_[i] = ~words_[i];
  }
  words_[size_ - 1] &= tail_mask_;
}

int64_t BitSet::NbActives() {
  int64_t res = 0;
  for (size_t i = 0; i < size_; ++i) {
    res += NbOfBitSets(words_[i]);
  }
  return res;
}

void BitSet::Print(std::ostream &ss) {
  ss << "words = ";
  for (size_t i = 0; i < size_; ++i) {
    PrintBinary(words_[i], ss);
    ss << " - ";
  }
  ss << " tail = ";
  PrintBinary(tail_mask_, ss);
  ss << "\n";
}

uint64_t NbOfConsecutiveZeros(uint64_t w) {
  uint64_t c;
  if (w & 0x1) {
    c = 0;
  } else {
    c = 1;
    if ((w & 0xffffffff) == 0) {
      w >>= 32;
      c += 32;
    }
    if ((w & 0xffff) == 0) {
      w >>= 16;
      c += 16;
    }
    if ((w & 0xff) == 0) {
      w >>= 8;
      c += 8;
    }
    if ((w & 0xf) == 0) {
      w >>= 4;
      c += 4;
    }
    if ((w & 0x3) == 0) {
      w >>= 2;
      c += 2;
    }
    c -= w & 0x1;
  }
  return c;
}

/**
 * Return the number of bits set to 1 in word :arg w:
 **/
uint64_t NbOfBitSets(uint64_t w){
  return  BitsSetTable256[w & 0xff] + 
          BitsSetTable256[(w >> 8) & 0xff] + 
          BitsSetTable256[(w >> 16) & 0xff] + 
          BitsSetTable256[(w >> 24) & 0xff] +
          BitsSetTable256[(w >> 32) & 0xff] + 
          BitsSetTable256[(w >> 40) & 0xff] + 
          BitsSetTable256[(w >> 48) & 0xff] + 
          BitsSetTable256[(w >> 56) & 0xff]; 
}

}  // namespace MDD
