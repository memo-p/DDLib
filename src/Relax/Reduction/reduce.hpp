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

#ifndef SRC_RELAX_REDUCTION_REDUCE
#define SRC_RELAX_REDUCTION_REDUCE

#include <stdio.h>

#include <map>
#include <queue>
#include <set>

#include "Core/MDD.hpp"
#include "DataStructures/SparseSet.hpp"
#include "Relax/Partitioner.hpp"
#include "Utils/ChronoP.hpp"

namespace MDD {

/**
 * Reduction of the relax MDD.
 * For a given maximum @param width, and a selection function, this
 * operation enforces that no more than @param nodes are present in
 * each layer of the MDD.
 **/
class RelaxReduce {
 private:
  MDD &mdd_;

  /**
   * Maximum layer width.
   **/
  const int width_relax_;

  /**
   * Mapping between the sets and the newly created nodes.
   **/
  std::map<std::set<Node *>, Node *> sets_to_node_;
  std::map<Node *, std::set<Node *>> nodes_to_set_;

  /**
   * Local vector that is given to the select function and contains nodes that
   * need to be merged together.
   **/
  std::set<Node *> to_merge_;
  /**
   * Local vector that is tre union of the set nodes of the nodes to merge.
   **/
  std::set<Node *> to_merge_union_;

  /**
   * StopWatch for the build.
   **/
  ChronoP chrono_;

  /**
   * Arc vector for the merge.
   **/
  std::vector<Arc *> arcs_;

  /**
   * Set of the ending nodes for the union.
   **/
  std::set<Node *> ends_;

  /**
   * Node that should not be taken into account by the select
   * cause they have no incomming arcs but are needed for the
   * creation of the outgoing arcs of newly created sets of nodes.
   **/
  std::vector<Node *> to_del_nodes_;

  /**
   * Local vector of the newly created nodes.
   **/
  std::vector<Node *> new_nodes_;

  /**
   * Partitions of the nodes of a layer.
   **/
  std::vector<std::set<Node *>> partition_;

  /**
   * Node to partition mapping.
   **/
  std::unordered_map<int, int> node_to_partition_;

  /**
   * Heuristic of partition for a layer.
   **/
  Partitioner *partitioner_;

 public:
  RelaxReduce(MDD &mdd, const int max_width, Partitioner *partitioner,
              bool apply = true)
      : mdd_(mdd), width_relax_(max_width), partitioner_(partitioner) {
    assert(partitioner != nullptr);
    new_nodes_.reserve(max_width);
    partition_.resize(max_width);
    // init set data structure with singleton.
    for (int layer = 1; layer <= mdd_.Size(); layer++) {
      Node *n = mdd_.getNodeLvl(layer);
      while (n) {
        nodes_to_set_[n].insert(n);
        sets_to_node_[nodes_to_set_[n]] = n;
        n = n->Next();
      }
    }
    if (apply) {
      Reduce();
    }
  }

  void Reduce() {
    chrono_.Start();
    for (int layer = 1; layer < mdd_.Size(); layer++) {
      ReduceLayer(layer);
    }
    chrono_.Stop();
  }

  void ReduceLayer(int layer, bool clean = true) {
    if (mdd_.GetLayer(layer).Size() <= WidthRelax()) {
      // Build the "ends" nodes of the previous interation.
      BuildOutgoingArcs(layer);
      return;
    }

    // starts by merging nodes until no more merge are required.
    for (auto &&p : partition_) {
      p.clear();
    }

    partitioner_->Partition(mdd_.GetLayer(layer), partition_);
    BuildNodeForPartitions();

    // Build the outgoing arcs only of 1 node for each partition.
    for (Node *n : new_nodes_) {
      if (n && !n->Arcs()) {
        BuildOutgoingArcs(n);
      }
    }

    {  // Update the layer above.
      const int layer_above = layer - 1;
      Node *n = mdd_.getNodeLvl(layer_above);
      while (n) {
        Arc *a = n->Arcs();
        while (a) {
          n->updateArc(a, Leader(a->End()));
          a = a->Next();
        }
        n = n->Next();
      }
    }
    if (clean) {
      CleanLayer(layer);
    }
  }

  /**
   * Remove the nodes of a layer that are no longer linked to the MDD.
   **/
  void CleanLayer(int layer) {
    {  // remove old nodes.
      // careful because some member of partition may be the final node.
      // Find nodes in the next layer that no longer have incoming arcs.
      Node *n = mdd_.getNodeLvl(layer);
      while (n) {
        Node *next = n->Next();
        if (n->InDegree() == 0) {
          Arc *a = n->Arcs();
          while (a) {
            Arc *a_next = a->Next();
            n->deleteArc(a);
            a = a_next;
          }  // TODO put the arc removal in the node removal.
          mdd_.DeleteNode(n);
        }
        n = next;
      }
    }

    {  // Remove Nodes that are no longer used.
      // Because of the previous iteration
      for (Node *n : to_del_nodes_) {
        Arc *a = n->Arcs();
        while (a) {
          Arc *a_next = a->Next();
          n->deleteArc(a);
          a = a_next;
        }
        delete n;
      }
      to_del_nodes_.clear();
    }

    {  // Find nodes in the next layer that no longer have incoming arcs.
      const int layer_below = layer + 1;
      if (layer_below != mdd_.Size()) {
        Node *n = mdd_.getNodeLvl(layer_below);
        while (n) {
          Node *next = n->Next();
          if (n->InDegree() == 0) {
            to_del_nodes_.push_back(n);
            mdd_.removeList(n);
          }
          n = next;
        }
      }
    }
  }

  /**
   * Build the outgoing arcs of all the nodes of the layer @param layer.
   * Only if they do not possess any arc.
   **/
  void BuildOutgoingArcs(int layer) {
    Node *n = mdd_.getNodeLvl(layer);
    while (n) {
      if (!n->Arcs()) {
        BuildOutgoingArcs(n);
      }
      n = n->Next();
    }
  }

  /**
   * Build the outgoing arcs of a given set of nodes.
   * This is basically a union operation.
   **/
  void BuildOutgoingArcs(Node *n) {
    std::set<Node *> &nodes = nodes_to_set_[n];
    assert(nodes.size() != 0);
    arcs_.clear();
    for (Node *n : nodes) {
      if (n->Arcs()) {
        arcs_.push_back(n->Arcs());
      }
    }
    while (arcs_.size()) {
      // 1) find the smallest label.
      int min_label = std::numeric_limits<int>::max();
      for (const Arc *a : arcs_) {
        min_label = std::min(min_label, a->Value());
      }
      // 2) create an arc labeled by min_label directed to
      // the union of the sets of arcs ending extremities.
      ends_.clear();
      for (size_t i = 0; i < arcs_.size(); i++) {
        if (arcs_[i]->Value() == min_label) {
          ends_.insert(arcs_[i]->End());
          arcs_[i] = arcs_[i]->Next();
          if (!arcs_[i]) {
            arcs_[i] = arcs_.back();
            arcs_.pop_back();
            i--;
          }
        }
      }
      Node *end = GetNodeForSet(ends_);
      // Arcs of end will be created only if he is selected as
      // singleton in his partition.
      n->AddArcLast(min_label, end);
    }
  }

  void BuildNodeForPartitions() {
    new_nodes_.clear();
    new_nodes_.resize(WidthRelax());
    for (size_t i = 0; i < partition_.size(); i++) {
      if (partition_[i].size() > 0) {
        std::set<Node *> part;
        for (Node *n : partition_[i]) {  // extract the unit nodes.
          part.insert(nodes_to_set_[n].begin(), nodes_to_set_[n].end());
          ;
        }
        new_nodes_[i] = GetNodeForSet(part);
        for (Node *n : partition_[i]) {  // update leader.
          n->newV_ = new_nodes_[i];
        }
      }
    }
  }

  /**
   * Given a set of nodes, return the node representing them.
   **/
  Node *GetNodeForSet(std::set<Node *> &nodes) {
    assert(nodes.size() != 0);
    if (nodes.size() == 1) {
      return *nodes.begin();
    }
    Node *n = sets_to_node_[nodes];
    if (n == nullptr) {
      Node *elem = *nodes.begin();
      n = mdd_.createNode(elem->VarIndex());
      sets_to_node_[nodes] = n;
      nodes_to_set_[n] = nodes;
    }
    return n;
  }

  /**
   * Return the maximum allowed number of nodes in a layer.
   **/
  int WidthRelax() const { return width_relax_; }

  /**
   * Find the nodes that replaced the current node.
   * Since several merges may have been performed,
   * an update of the newV is done in the spirit of the
   * union find.
   **/
  Node *Leader(Node *n) {
    if (n == n->newV_ || n->newV_ == nullptr) {
      return n->newV_;
    }
    return n->newV_ = Leader(n->newV_);
  }

  /**
   * Return the set of nodes represented by a new node,
   * or the empty set for the old ones.
   **/
  std::set<Node *> &NodesForNode(Node *n) { return nodes_to_set_[n]; }
};

}  // namespace MDD

#endif /* SRC_RELAX_REDUCTION_REDUCE */
