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

#ifndef SRC_DATASTRUCTURES_LIST
#define SRC_DATASTRUCTURES_LIST
#include <cassert>

namespace MDD {

/**
 * @brief List element class.
 * 
 * This class mainly add the fields @p next_ and 
 * @p prev_ to the class @p T.
 * 
 * @tparam T Type of object to be linked.
 */
template <class T>
class DblListElement {
 public:
  T* next_;
  T* prev_;

  DblListElement() : next_(nullptr), prev_(nullptr) {}
  /**
   * @brief Put element @p elem next to this.
   * 
   * Complexity: Constant time.
   **/
  void AddNextList(T* elem) {
    elem->next_ = next_;
    elem->prev_ = (T*)this;
    if (next_ != nullptr) {
      next_->prev_ = elem;
    }
    next_ = elem;
  }

  /**
   * @brief Put element @p elem before this.
   * 
   * Complexity: Constant time.
   **/
  void AddPrevList(T* elem) {
    elem->next_ = (T*)this;
    elem->prev_ = prev_;
    if (prev_ != nullptr) {
      prev_->next_ = elem;
    }
    prev_ = elem;
  }

  /**
   * @brief Remove this element from the list.
   * 
   * Complexity: Constant time.
   **/
  void RemoveList() {
    if (prev_ != nullptr) {
      prev_->next_ = next_;
    }
    if (next_ != nullptr) {
      next_->prev_ = prev_;
    }
    next_ = nullptr;
    prev_ = nullptr;
  }
  /**
   * @brief Return the next element in the list.
   * 
   * @return T* next element or nullptr.
   * 
   * Complexity: Constant time.
   */
  T* Next() { return next_; }
  /**
   * @brief Return the previous element in the list.
   * 
   * @return T* previous element or nullptr.
   * 
   * Complexity: Constant time.
   */
  T* Prev() { return prev_; }
};

/**
 * @brief List of @p DblListElement<T> elements.
 * 
 * @tparam T Class of the elements.
 */
template <class T>
class DblList {
 public:
  T* first_;
  T* last_;
  int size_;

  DblList() : first_(nullptr), last_(nullptr), size_(0) {}

  /**
   * @brief Add element @p elem at the beginning of this list.
   * 
   * Complexity: Constant time.
   **/
  void AddFirst(T* elem) {
    assert(elem->Next() == nullptr);
    assert(elem->Prev() == nullptr);
    assert(size_ >= 0);
    elem->next_ = first_;
    if (size_) {
      first_->prev_ = elem;
    } else {
      last_ = elem;
    }
    first_ = elem;
    size_++;
    assert(size_ >= 0);
  }

  /**
   * @brief Add element @p elem at the end of this list.
   * 
   * Complexity: Constant time.
   **/
  void AddLast(T* elem) {
    assert(elem->Next() == nullptr);
    assert(elem->Prev() == nullptr);
    if (size_) {
      elem->prev_ = last_;
      last_->next_ = elem;
    } else {
      first_ = elem;
    }
    last_ = elem;
    size_++;
    assert(size_ >= 0);
  }

  /**
   * @brief Add @p new_elem after @p present_elem in this list.
   * 
   * Complexity: Constant time.
   **/
  void AddAfter(T* new_elem, T* present_elem) {
    assert(new_elem->Next() == nullptr);
    assert(new_elem->Prev() == nullptr);
    assert(size_ > 0);
    assert(present_elem != nullptr);

    new_elem->prev_ = present_elem;
    new_elem->next_ = present_elem->next_;
    if (present_elem == last_) {
      last_ = new_elem;
    } else {
      present_elem->next_->prev_ = new_elem;
    }
    present_elem->next_ = new_elem;
    size_++;
    assert(size_ >= 0);
  }  
  
  /**
   * @brief Add @p new_elem before @p present_elem.
   * 
   * Complexity: Constant time.
   **/
  void AddBefore(T* new_elem, T* present_elem) {
    assert(new_elem->Next() == nullptr);
    assert(new_elem->Prev() == nullptr);
    assert(size_ > 0);
    assert(present_elem != nullptr);

    new_elem->prev_ = present_elem->prev_;
    new_elem->next_ = present_elem;
    if (present_elem == first_) {
      first_ = new_elem;
    } else {
      present_elem->prev_->next_ = new_elem;
    }
    present_elem->prev_ = new_elem;
    size_++;
    assert(size_ >= 0);
  }

  /**
   * @brief Remove element @p elem of this list.
   * 
   * Complexity: Constant time.
   **/
  void Remove(T* elem) {
    if (elem->next_ != nullptr) {
      elem->next_->prev_ = elem->prev_;
    } else {  // last_
      last_ = elem->prev_;
    }
    if (elem->prev_ != nullptr) {
      elem->prev_->next_ = elem->next_;
    } else {  // first_
      first_ = elem->next_;
    }
    elem->next_ = nullptr, elem->prev_ = nullptr, size_--;
    assert(size_ >= 0);
  }

  /**
   * @brief Append the list @p l at the end of this list.
   * Note that this will clear list @p l after.
   * 
   * Complexity: Constant time.
   **/
  void Append(DblList<T>& l) {
    if (size_) {
      if (l.size_) {
        last_->AddNextList(l.first_);
        last_ = l.last_;
      }
    } else {
      first_ = l.first_;
      last_ = l.last_;
    }
    size_ += l.size_;
    l.Clear();
    assert(size_ >= 0);
  }

  /**
   * @brief Reset the data structures of the list.
   * The list will be emptied.
   * 
   * Complexity: Constant time.
   **/
  void Clear() {
    first_ = nullptr;
    last_ = nullptr;
    size_ = 0;
  }

  /**
   * @brief Return the first element of the list.
   * 
   * The next element is accessible through method @p DblListElement<T>::Next() .
   * 
   * Complexity: Constant time.
   */
  T* First() const { return first_; }
  /**
   * @brief Return the last element of the list.
   * 
   * The previous element is accessible through method @p DblListElement<T>::Prev() .
   * 
   * Complexity: Constant time.
   */
  T* Last() const { return last_; }

  /**
   * @brief return the number of element of the list.
   * 
   * Complexity: Constant time.
   */
  int Size() const { return size_; }
};

}  // namespace MDD

#endif /* SRC_DATASTRUCTURES_LIST */
