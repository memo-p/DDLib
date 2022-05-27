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

#ifndef SRC_DATASTRUCTURES_ORDEREDLIST
#define SRC_DATASTRUCTURES_ORDEREDLIST

#include "DataStructures/list.hpp"

namespace MDD {

/**
 * @brief Increasingly ordered list template.
 * 
 * @tparam T Type of object to be linked. 
 * This type must have methods Value().
 **/
template <class T>
class DblOrderedList : public DblList<T> {

 public:
  
  /**
   * @brief Add element @p elem to this list.
   * 
   * @param elem Element to be added.
   * 
   * Complexity: Linear time over the number of element in the  worst-case.
   * 
   */
  void Add(T* elem) {
    if (DblList<T>::size_) {  // List is not empty
      T* e = DblList<T>::first_;
      while (e && e->Value() < elem->Value()) {  // looking for the 1st larger
        e = e->next_;
      }
      if (e) {  // IF 1st larger or equal exists
          e->AddPrevList(elem);
          if (elem->prev_ == nullptr) {  // The first one larger
            DblList<T>::first_ = elem;
          }
      } else {  // the last one is smaller
        elem->prev_ = DblList<T>::last_;
        DblList<T>::last_->next_ = elem;
        DblList<T>::last_ = elem;
      }
    } else {
      DblList<T>::first_ = elem;
      DblList<T>::last_ = elem;
    }
    DblList<T>::size_++;
  }
  
  /**
   * @brief Find element with value @p value in this list.
   * 
   * @param value Value of the element to find.
   * 
   * Complexity: Linear time over the number of element in the  worst-case.
   * 
   */
  T* Find(int value) const{
    T* e = DblList<T>::first_;
    while (e && e->Value() != value) {
      e = e->next_;
    }
    return e;
  }
};
}  // namespace MDD

#endif /* SRC_DATASTRUCTURES_ORDEREDLIST */
