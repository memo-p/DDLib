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

#ifndef SRC_CORE_MDD
#define SRC_CORE_MDD

#include <stdio.h>
#include <stdlib.h>

#include <thread>
#include <vector>

#include "DataStructures/list.hpp"
#include "DataStructures/orderedList.hpp"

#define NBALLOC 1000000
namespace MDD {

class Node;
/**
 * Base class of the MDD arcs.
 **/
class Arc : public DblListElement<Arc> {
 public:
  static thread_local Arc *allocated;

  void *operator new(std::size_t count);

  void operator delete(void *ptr);

  Arc(Node *start, Node *end, int value)
      : start_(start), end_(end), red_arc_(nullptr), value_(value) {}

  /**
   * Return the label of the arc.
   **/
  int Value() const { return value_; }
  /**
   * Return the starting node of the arc.
   **/
  Node *Start() const { return start_; }
  /**
   * Return the ending node of the arc.
   **/
  Node *End() const { return end_; }

  Node *start_;   // starting node
  Node *end_;     // ending node
  Arc *red_arc_;  // tmp arc for apply
  int value_;     // label
  int info1_;     // unique id
};

class Node : public DblListElement<Node> {
 public:
  static thread_local Node *allocated;

  void *operator new(std::size_t count);
  void operator delete(void *ptr);

  /**
   * Add an arc labeled by @param value and directed to
   * @param n.
   * A search for the position of the arc is done linearly.
   *
   * If an arc with label @param value already exists,
   * its destination is updated.
   **/
  Arc *AddArc(int value, Node *n);

  /**
   * Add an arc labeled by @param value and directed to
   * @param n.
   * No search is done, the arc is directly put on the back of the arc list.
   **/
  void AddArcLast(int value, Node *n);

  /**
   * Add an arc labeled by @param value and directed to
   * @param n.
   * No search is done, the arc is directly put on the front of the arc list.
   **/
  void AddArcFirst(int value, Node *n);

  /**
   * Add an arc labeled by @param value and directed to
   * @param n.
   * No search is done, the arc is directly put after @param a.
   **/
  void AddArcAfter(int value, Node *n, Arc *a);

  
  /**
   * Add an arc labeled by @param value and directed to
   * @param n.
   * No search is done, the arc is directly put before @param a.
   **/
  void AddArcBefore(int value, Node *n, Arc *a);

  /**
   * Return the arc labeled by @param value
   * if it exists, nullptr otherwise.
   **/
  Arc *get(int value) const;

  /**
   * Update the destination of a given arc
   **/
  void updateArc(Arc *e, Node *newEnd);

  /**
   * Remove the arc @param e from the arc list.
   * Note that the function also delete @param e.
   **/
  void deleteArc(Arc *e);

  /**
   * return the arc with the smallest value.
   **/
  Arc *Arcs() const { return arcs_.First(); }

  /**
   * return the arc with the Largest value.
   **/
  Arc *lastArc() const { return arcs_.Last(); }

  /**
   * return the index of the variable represented by the node.
   **/
  int VarIndex() const { return var_index_; }

  /**
   *
   **/
  std::string to_string();
  int OutDegree() const { return arcs_.Size(); }
  int InDegree() const { return nb_in_; }
  int UID() const { return id_; }

  Node(int varIndex, int id)
      : id_(id),
        var_index_(varIndex),
        info1_(0),
        info2_(0),
        info3_(0),
        nb_in_(0),
        visited_(false),
        red_arc_(nullptr),
        red_next_(nullptr),
        newV_(this) {}

  int id_;         // unique id
  int var_index_;  // index of the var represented by the node
  int info1_;      // additional info if needed (used in some algorithms)
  int info2_;      // additional info if needed (used in some algorithms)
  int info3_;      // additional info if needed (used in some algorithms)
  int nb_in_;      // number of incoming arcs
  bool visited_;   // boolean used for DFS/BFS
  DblOrderedList<Arc> arcs_;  // ORDORED list of outgoing arcs
  Arc *red_arc_;              // arc currently studied in the reduction
  Node *red_next_;            // list of node for the reduction
  Node *newV_;                // new version after the merge
};

class MDD {
 public:
  /**
   * Base method for creating a Node.
   **/
  Node *createNode(int var_id);

  /**
   * Add a node to the list of nodes of its layer.
   * If the node has been created outside of this MDD
   **/
  void addList(Node *n);

  /**
   * Remove a node to the list of nodes of its layer.
   **/
  void removeList(Node *n);

  /**
   * delete a node from the MDD and delete the ptr.
   **/
  void DeleteNode(Node *n);

  /**
   * Remove node that are redundant
   **/
  void clean();

  /**
   * Extract the layer @param layer from mdd @param mdd
   * and insert it into this.
   **/
  void addLayer(MDD &mdd, int layer);

  ~MDD() { clear(); }

  /**
   * Remove all the arcs and nodes of the MDD.
   *
   * Called when the MDD need to be destroyed
   **/
  void clear();

  /**
   * Remove all the nodes present in the toDel array
   **/
  void clearDel();

  /**
   * Return the number of layers.
   **/
  int getSize() const { return size_; }
  int Size() const { return size_; }

  /**
   * Return the first node of the layer @param lvl.
   **/
  Node *getNodeLvl(int lvl) const { return nodes_[lvl].First(); }
  const DblList<Node>& GetLayer(int lvl) const { return nodes_[lvl]; }

  /**
   * Set the domain size of each layer individually
   **/
  void setDomSize(std::vector<int> domSize);

  /**
   * Set the domain size of each layer individually
   **/
  void setDomSize(int *domSize);

  /**
   * Set the domain size to be the same for all the variables.
   **/
  void setDomSize(int domSize);

  /**
   * Return the number of node of the largest layer.
   **/
  int largestLayerSize() const;

  /**
   * Return the largest domain size.
   **/
  int largestDomainSize() const {
    assert(maxDomSize_ >= 0);
    return maxDomSize_;
  };

  /**
   * Create root node and a final node.
   **/
  void BuildRootAndFinalNodes();

  /**
   * Return true if the MDD contains such a tuple.
   **/
  bool contains(std::vector<int> const &tuple) const;

  Node *Root() const { return nodes_.front().First(); }
  Node *Final() const { return nodes_.back().Last(); }

  MDD(int size)
      : nodes_(size + 1),
        domSize_(size),
        toDel(nullptr),
        size_(size),
        node_id_(0),
        maxDomSize_(-1),
        nb_nodes_(0),
        nb_arcs_(0) {}

  std::vector<DblList<Node>> nodes_;  // vector of lists of nodes
  std::vector<int> domSize_;  // domain size of the variable  [0, domSize_(i)[
  Node *toDel;                // buffer of node to del
  int size_;                  // number of lvl
  int node_id_;               // current max ID
  int maxDomSize_;            // Maximal size of the domaine
  int nb_nodes_;              // number of nodes
  int nb_arcs_;               // number of arcs
};

}  // namespace MDD

#endif /* SRC_CORE_MDD */
