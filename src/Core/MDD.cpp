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

#include "MDD.hpp"

namespace MDD {

thread_local Arc *Arc::allocated = nullptr;
thread_local Node *Node::allocated = nullptr;

//////////////////////////
//////////// ARC ////////
////////////////////////

void *Arc::operator new(std::size_t count) {
  if (allocated == nullptr) {
    char *m = (char *)::new char[count * NBALLOC];
    Arc *e = (Arc *)m;
    allocated = (Arc *)m;
    for (int i = 0; i < NBALLOC - 1; i++) {
      e->next_ = (Arc *)(m + count * (i + 1));
      e = e->next_;
    }
    e->next_ = nullptr;
  }
  Arc *retour = allocated;
  allocated = allocated->next_;
  return retour;
}

void Arc::operator delete(void *ptr) {
  ((Arc *)ptr)->next_ = allocated;
  allocated = (Arc *)ptr;
}

//////////////////////////
/////////// NODE ////////
////////////////////////

void *Node::operator new(std::size_t count) {
  if (allocated == nullptr) {
    char *m = (char *)::new char[count * NBALLOC];
    Node *e = (Node *)m;
    allocated = (Node *)m;
    for (int i = 0; i < NBALLOC - 1; i++) {
      e->next_ = (Node *)(m + count * (i + 1));
      e = e->next_;
    }
    e->next_ = nullptr;
  }
  Node *retour = allocated;
  allocated = allocated->next_;
  return retour;
}
void Node::operator delete(void *ptr) {
  ((Node *)ptr)->next_ = allocated;
  allocated = (Node *)ptr;
}

Arc *Node::AddArc(int value, Node *n) {
  Arc *new_arc = new Arc(this, n, value);
  arcs_.Add(new_arc);
  n->nb_in_++;
  return new_arc;
}

void Node::AddArcLast(int value, Node *n) {
  arcs_.AddLast(new Arc(this, n, value));
  n->nb_in_++;
}

void Node::AddArcFirst(int value, Node *n) {
  arcs_.AddFirst(new Arc(this, n, value));
  n->nb_in_++;
}

void Node::AddArcAfter(int value, Node *n, Arc *a) {
  arcs_.AddAfter(new Arc(this, n, value), a);
  n->nb_in_++;
}

void Node::AddArcBefore(int value, Node *n, Arc *a) {
  arcs_.AddBefore(new Arc(this, n, value), a);
  n->nb_in_++;
}

Arc *Node::get(int value) const { return arcs_.Find(value); }

void Node::updateArc(Arc *e, Node *newEnd) {
  e->end_->nb_in_--;
  if (newEnd == nullptr) {
    deleteArc(e);
  } else {
    e->end_ = newEnd;
    newEnd->nb_in_++;
  }
}

void Node::deleteArc(Arc *e) {
  arcs_.Remove(e);
  e->end_->nb_in_--;
  delete e;
}

std::string Node::to_string() {
  std::string s = "";
  Arc *a = Arcs();
  while (a) {
    s += "(" + std::to_string(a->Value()) + "," +
         std::to_string(a->End()->UID()) + ")";
    a = a->Next();
  }
  return s;
}

//////////////////////////
/////////// MDD /////////
////////////////////////
Node *MDD::createNode(int var_id) {
  Node *n = new Node(var_id, node_id_++);
  addList(n);
  return n;
}

void MDD::addList(Node *n) { nodes_[n->VarIndex()].AddFirst(n); }

void MDD::removeList(Node *n) {
  nodes_[n->VarIndex()].Remove(n);
}

void MDD::DeleteNode(Node *n) {
  removeList(n);
  delete n;
}

void MDD::clean() {
  for (int l = 0; l < size_; l++) {
    Node *n = getNodeLvl(l);
    while (n != nullptr) {
      Node *n_next = n->next_;
      if (n->newV_ != n) {
        removeList(n);
      }
      n = n_next;
    }
  }
}

void MDD::addLayer(MDD &mdd, int layer) {
  nodes_[layer].Append(mdd.nodes_[layer]);
  mdd.nodes_[layer].Clear();
}

void MDD::clear() {
  Node *tmp;
  Node *tmp2;
  for (int i = 0; i <= size_; i++) {
    tmp = nodes_[i].First();
    nodes_[i].Clear();
    while (tmp != nullptr) {
      tmp2 = tmp->next_;
      delete tmp;
      tmp = tmp2;
    }
  }
}

void MDD::clearDel() {
  Node *tmp;
  while (toDel != nullptr) {
    tmp = toDel->next_;
    delete toDel;
    toDel = tmp;
  }
}

void MDD::setDomSize(std::vector<int> domSize) {
  maxDomSize_ = 0;
  for (int i = 0; i < size_; i++) {
    domSize_[i] = domSize[i];
    maxDomSize_ = (maxDomSize_ < domSize[i]) ? domSize[i] : maxDomSize_;
  }
}

void MDD::setDomSize(int *domSize) {
  maxDomSize_ = 0;
  for (int i = 0; i < size_; i++) {
    domSize_[i] = domSize[i];
    maxDomSize_ = (maxDomSize_ < domSize[i]) ? domSize[i] : maxDomSize_;
  }
}

void MDD::setDomSize(int domSize) {
  maxDomSize_ = domSize;
  for (int i = 0; i < size_; i++) {
    domSize_[i] = domSize;
  }
}

int MDD::largestLayerSize() const {
  int maxS = 0;
  for (int layer = 0; layer < getSize(); layer++) {
    maxS = std::max(nodes_[layer].Size(), maxS);
  }
  return maxS;
}

bool MDD::contains(std::vector<int> const &tuple) const {
  Node *n = Root();
  Arc *a = nullptr;
  for (int v : tuple) {
    a = n->get(v);
    if (a == nullptr) {
      return false;
    }
    assert(a->Value() == v);
    n = a->End();
  }
  return a != nullptr;
}

void MDD::BuildRootAndFinalNodes() {
  createNode(0);
  createNode(Size());
}

}  // namespace MDD
