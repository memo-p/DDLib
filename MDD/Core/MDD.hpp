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

// Number of consecutive allocations to be made in the new.
#define NBALLOC 1000000

namespace MDD {

class Node;

/**
 * Base class of the MDD arcs.
 **/
class Arc : public DblListElement<Arc> {
 public:
 /**
  * @brief Link to the previously allocated nodes.
  * Used by the new and delete redefinition.
  */
  static thread_local Arc *allocated;

  void *operator new(std::size_t count);

  void operator delete(void *ptr);

  /**
   * @brief Construct a new Arc object.
   * 
   * @param start starting node of the arc.
   * @param end ending node of the arc.
   * @param value label of the arc.
   */
  Arc(Node *start, Node *end, int value)
      : start_(start), end_(end), red_arc_(nullptr), value_(value) {}

  /**
   * Return the value (label) of the arc.
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

  /**
   * @brief Starting node of the arc.
   */
  Node *start_;
  /**
   * @brief Ending node of the arc.
   */
  Node *end_;
  /**
   * @brief Temporary arc used by the Apply operator.
   */
  Arc *red_arc_;
  /**
   * @brief Value (label) of the arc.
   */
  int value_;
};

/**
 * @brief Base class for the node of an MDD.
 * 
 */
class Node : public DblListElement<Node> {
 public:
  /**
  * @brief Link to the previously allocated nodes.
  * Used by the new and delete redefinition.
  */
  static thread_local Node *allocated;

  void *operator new(std::size_t count);
  void operator delete(void *ptr);

  /**
   * @brief Add an arc labeled by @p value and directed to
   * @p n to the current outgoing arcs of this node.
   *
   * If an arc with label @p value already exists,
   * its destination is updated.
   *
   * Complexity: A search for the position of the arc is done linearly.
   *
   * @param value Value (label) of the arc.
   * @param n Ending node.
   **/
  Arc *AddArc(int value, Node *n);

  /**
   * @brief Add an arc labeled by @p value and directed to
   * @p n to the current outgoing arcs of this node.
   *
   * Complexity: No search is done, the arc is directly put on the back of the
   * arc list.
   *
   * @param value Value (label) of the arc.
   * @param n Ending node.
   **/
  void AddArcLast(int value, Node *n);

  /**
   * @brief Add an arc labeled by @p value and directed to
   * @p n to the current outgoing arcs of this node.
   * 
   * Complexity: No search is done, the arc is directly put on the front of the arc list.
   * 
   * @param value Value (label) of the arc.
   * @param n Ending node.
   **/
  void AddArcFirst(int value, Node *n);

  /**
   * @brief Add an arc labeled by @p value and directed to
   * @p n after arc @p a in outgoing arcs list of this node.
   * 
   * Complexity: No search is done, the arc is directly put after Arc @p a.
   * 
   * @param value Value (label) of the arc.
   * @param n Ending node.
   * @param a arc in the list of outgoing arcs of this node. 
   **/
  void AddArcAfter(int value, Node *n, Arc *a);

  
  /**
   * @brief Add an arc labeled by @p value and directed to
   * @p n before arc @p a in outgoing arcs list of this node.
   * 
   * Complexity: No search is done, the arc is directly put before @p a.
   * @param value Value (label) of the arc.
   * @param n Ending node.
   * @param a arc in the list of outgoing arcs of this node. 
   **/
  void AddArcBefore(int value, Node *n, Arc *a);

  /**
   * @brief Return the arc labeled by @p value
   * if it exists, nullptr otherwise.
   * 
   * Complexity: A search for the position of the arc is done linearly.
   * 
   * @param value Value (label) of the arc.
   **/
  Arc *get(int value) const;

  /**
   * @brief Change the ending node of arc @p a to node @p newEnd.
   * 
   * Complexity: Constant time.
   * 
   * @param a arc in the list of outgoing arcs of this node. 
   * @param newEnd The new ending node.
   **/
  void updateArc(Arc *a, Node *newEnd);

  /**
   * @brief Remove the arc @p a from the arc list.
   * Note that the function also delete @p a.
   * 
   * Complexity: Constant time.
   * 
   * @param a arc in the list of outgoing arcs of this node. 
   **/
  void deleteArc(Arc *a);

  /**
   * @brief Return the first arc of the outgoing arcs of this node.
   * 
   * Note that the arc is the one with the smallest value.
   * 
   * Complexity: Constant time.
   **/
  Arc *Arcs() const { return arcs_.First(); }

  /**
   * @brief Return the last arc of the outgoing arcs of this node.
   * 
   * Note that the arc is the one with the largest value.
   * 
   * Complexity: Constant time.
   **/
  Arc *lastArc() const { return arcs_.Last(); }

  /**
   * @brief Return the index of the variable represented by the node.
   * 
   * Complexity: Constant time.
   **/
  int VarIndex() const { return var_index_; }

  /**
   * @brief Conversion of the node into a string describing it.
   * 
   * Note that equivalent node have the same string.
   * 
   * Complexity: Linear time with respect to the number of arcs.
   **/
  std::string to_string();

  /**
   * @brief Return the number of outgoing arcs.
   * 
   * Complexity: Constant time.
   * 
   * @return int the number of outgoing arcs.
   */
  int OutDegree() const { return arcs_.Size(); }

  /**
   * @brief Return the number of incoming arcs.
   * 
   * Complexity: Constant time.
   * 
   * @return int the number of incoming arcs.
   */
  int InDegree() const { return nb_in_; }

  /**
   * @brief Return the unique id of the node.
   * 
   * Complexity: Constant time.
   * 
   * @return int the unique id of the node.
   */
  int UID() const { return id_; }

/**
 * @brief Construct a new Node object.
 * 
 * @param varIndex Index of the variable that this node represents.
 * @param id Unique id of the node.
 */
  Node(int varIndex, int id)
      : id_(id),
        var_index_(varIndex),
        info1_(0),
        nb_in_(0),
        visited_(false),
        newV_(this) {}

  int id_;         // unique id
  int var_index_;  // index of the var represented by the node
  int info1_;      // additional info if needed (used in some algorithms)
  int nb_in_;      // number of incoming arcs
  bool visited_;   // boolean used for DFS/BFS
  DblOrderedList<Arc> arcs_;  // ORDORED list of outgoing arcs
  Node *newV_;                // new version after the merge
};

/**
 * @brief Main Multi-valued Decision Diagram class of this Library.
 * 
 */
class MDD {
 public:
  /**
   * @brief Create a Node object.
   * 
   * Complexity: Constant time.
   * 
   * @param var_id Index of the variable that the returned node represents.
   * @return Node* A newly created node.
   */
  Node *createNode(int var_id);

  /**
   * @brief Add node @p n to the list of nodes of its layer.
   * 
   * Note that this method need to be called only 
   * If the node has been created outside of this MDD
   * 
   * Complexity: Constant time.
   * 
   * @param n Node to be added to the MDD.
   **/
  void addList(Node *n);

  /**
   * @brief Remove node @p n from the list of nodes of its layer.
   * 
   * Complexity: Constant time.
   * 
   * @param n Node to be removed from the MDD.
   **/
  void removeList(Node *n);

  /**
   * @brief Delete a node from the MDD and delete the ptr.
   * 
   * Complexity: Constant time.
   * 
   * @param n Node to be removed from the MDD and deleted.
   **/
  void DeleteNode(Node *n);

  /**
   * @brief Remove nodes that are redundant.
   * 
   * Complexity: Linear time with respect to the size 
   * of the MDD (i.e. nodes + arcs).
   **/
  void clean();
  // TODO(DEV) put in reduce.

  /**
   * @brief Extract the layer @p layer from mdd @p mdd
   * and insert it into this.
   * 
   * Note that the layer is then removed from @p mdd.
   * 
   * Complexity: Constant time.
   * 
   * @param mdd MDD from which the layer is extracted.
   * @param layer Id of the layer to be extracted.
   **/
  void addLayer(MDD &mdd, int layer);

  ~MDD() { clear(); }

  /**
   * @brief Remove all the arcs and nodes of the MDD.
   *
   * Note that this method is usually called when
   * the MDD need to be deleted.
   * Complexity: Linear time with respect to the size 
   * of the MDD (i.e. nodes + arcs).
   **/
  void clear();

  /**
   * @brief Remove all the nodes present in the toDel array.
   * 
   * Complexity: Linear time with respect to the size 
   * of the MDD (i.e. nodes + arcs).
   **/
  void clearDel();

  /**
   * @brief Return the number of layers.
   * 
   * Complexity: Constant time.
   **/
  int getSize() const { return size_; }
  /**
   * @brief Return the number of layers.
   * 
   * Complexity: Constant time.
   **/
  int Size() const { return size_; }
// TODO(dev) remove one.

  /**
   * @brief Return the first node of the layer @p layer.
   * 
   * Note that nodes of the same layer are linked together.
   * 
   * Complexity: Constant time.
   * 
   * @param layer Layer id.
   **/
  Node *getNodeLvl(int layer) const { return nodes_[layer].First(); }

  /**
   * @brief Return the list of nodes of the layer @p layer.
   * 
   * Complexity: Constant time.
   * 
   * @param layer Layer id.
   **/
  const DblList<Node>& GetLayer(int layer) const { return nodes_[layer]; }

  /**
   * @brief Set the domain size of each layer individually.
   * 
   * Complexity: Linear time with respect to the Size() of the MDD.
   * 
   * @param domSize Vector containing for each layer the number of values
   * the arcs can have.
   **/
  void setDomSize(std::vector<int> domSize);

  /**
   * @brief Set the domain size of each layer individually.
   * 
   * Complexity: Linear time with respect to the Size() of the MDD.
   * 
   * @param domSize Array containing for each layer the number of values
   * the arcs can have.
   **/
  void setDomSize(int *domSize);

  /**
   * @brief Set the domain size to be the same for all the variables.
   * 
   * Complexity: Linear time with respect to the Size() of the MDD.
   * 
   * @param domSize The number of values the arcs can have.
   **/
  void setDomSize(int domSize);

  /**
   * @brief Return the number of nodes of the largest layer.
   * 
   * Complexity: Linear time with respect to the Size() of the MDD.
   **/
  int largestLayerSize() const;

  /**
   * @brief Return the largest domain size.
   * 
   * Complexity: Constant time.
   **/
  int largestDomainSize() const {
    assert(maxDomSize_ >= 0);
    return maxDomSize_;
  };

  /**
   * @brief Create a root node and a final node.
   * 
   * Complexity: Constant time.
   **/
  void BuildRootAndFinalNodes();

  /**
   * @brief Return true if this MDD contains @p tuple .
   * 
   * Complexity: Linear time with respect to the Size() of the MDD.
   **/
  bool contains(std::vector<int> const &tuple) const;

  /**
   * @brief Return the root node of the MDD.
   */
  Node *Root() const { return nodes_.front().First(); }
  /**
   * @brief Return the final true node of the MDD.
   */
  Node *Final() const { return nodes_.back().Last(); }

  /**
   * @brief Construct a new MDD object.
   * 
   * @param size The number of layers of the MDD.
   */
  MDD(int size)
      : nodes_(size + 1),
        domSize_(size),
        toDel(nullptr),
        size_(size),
        node_id_(0),
        maxDomSize_(-1),
        nb_nodes_(0),
        nb_arcs_(0) {}

  /**
   * @brief A vector of the list of nodes of each layer.
   */
  std::vector<DblList<Node>> nodes_;
  /**
   * @brief A vector of the domain size of each layer.
   */
  std::vector<int> domSize_;
  Node *toDel;                // buffer of node to del
  int size_;                  // number of layers
  int node_id_;               // current max ID
  int maxDomSize_;            // Maximal size of the domaine
  int nb_nodes_;              // number of nodes
  int nb_arcs_;               // number of arcs
};

}  // namespace MDD

#endif /* SRC_CORE_MDD */
