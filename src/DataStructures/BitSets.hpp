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

#ifndef SRC_BITSET_BITSETS
#define SRC_BITSET_BITSETS

#include <cstdio>
#include <iostream>
#include <vector>

namespace MDD {

/**
 * Bitwise function
 * http://graphics.stanford.edu/~seander/bithacks.html
 */

/**
 * Return a word with all bits set to 0 except for the bit at position :arg pos:
 * */
inline uint64_t OneBit(uint64_t pos) { return 1ul << pos; }
/**
 * Return the word :arg work: with the bit at position :arg pos: set to 1
 * */
inline uint64_t SetBit(uint64_t word, uint64_t pos) {
  return word |= OneBit(pos);
}
/**
 * Return the word :arg work: with the bit at position :arg pos: set to 0
 * */
inline uint64_t UnsetBit(uint64_t word, uint64_t pos) {
  return word &= ~OneBit(pos);
}

void PrintBinary(uint64_t n, std::ostream &ss = std::cout);

/**
 * Compute the smallest power of two larger that :arg n:
 * */
uint64_t NextPowerOf2(uint64_t n);
/**
 * Return the word index of a position in a consecutive bitset
 * */
inline uint64_t WordIndex(uint64_t pos) { return pos >> 6; }
/**
 * Return the bit index of a position in a consecutive bitset
 * */
inline uint64_t BitIndex(uint64_t pos) { return pos & 63ul; }

/**
 * Return the minimum number of words required to store :arg n: bits
 * */
inline uint64_t NbWords(uint64_t n) { return WordIndex(n) + 1; }

/**
 * Return the number of bits set to 0 on the right of word :arg w:
 * */
uint64_t NbOfConsecutiveZeros(uint64_t w);

/**
 * Return the number of bits set to 1 in word :arg w:
 **/
uint64_t NbOfBitSets(uint64_t w);
/**
 * Array required by the NbOfBitSets function
 **/
static const unsigned char BitsSetTable256[256] = {
#define B2(n) n, n + 1, n + 1, n + 2
#define B4(n) B2(n), B2(n + 1), B2(n + 1), B2(n + 2)
#define B6(n) B4(n), B4(n + 1), B4(n + 1), B4(n + 2)
    B6(0), B6(1), B6(1), B6(2)};

class BitSet {
 private:
  BitSet() : BitSet(0){};

 protected:
  /**
   * Array of bitwords storing the set
   **/
  std::vector<uint64_t> words_;
  /**
   * Number of words in the set
   **/
  uint64_t size_;
  /**
   * Number of elements in the set
   **/
  uint64_t nb_element_;
  /**
   * Mask of the offset element at the end of the bitset
   **/
  uint64_t tail_mask_;

 public:
  BitSet(uint64_t nb_element, bool full = false)
      : words_(),
        size_(NbWords(nb_element)),
        nb_element_(nb_element),
        tail_mask_(0ul) {
    if (nb_element > 0) {
      resize(nb_element, full);
    } else {
      size_ = 0;
    }
  }

  bool Contains(uint64_t k) const;

  void Add(uint64_t k);

  void Remove(uint64_t k);

  void Restrict(uint64_t k);

  void Clear();

  void Intersection(const BitSet &bs);

  void Xor(const BitSet &bs);

  void Union(const BitSet &bs);
  void Not();

  uint64_t MaxNbElements() const { return nb_element_; }

  inline uint64_t operator[](int i) const { return words_[i]; }
  uint64_t GetSize() const { return size_; }

  void Print(std::ostream &ss = std::cout);

  std::vector<uint64_t> const & Words() const { return words_; }

  void resize(uint64_t nb_element, bool full = false) {
    size_ = NbWords(nb_element);
    nb_element_ = nb_element;
    tail_mask_ = 0ul;
    words_.resize(size_);
    if (full) {
      for (size_t i = 0; i < size_; ++i) {
        words_[i] = ~0ul;
      }
    }
    for (uint64_t i = 0; i < BitIndex(nb_element_); ++i) {
      tail_mask_ |= OneBit(i);
    }
    words_[size_ - 1] &= tail_mask_;
  }
};

}  // namespace MDD

#endif /* SRC_BITSET_BITSETS */
