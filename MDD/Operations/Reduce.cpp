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

#include "Reduce.hpp"

namespace MDD {
Reduce::Reduce(MDD& mdd)
    : mdd_(mdd),
      largest_layer_(mdd.largestLayerSize() + 1),
      largest_domain_(mdd.largestDomainSize() + 1),
      radix_size_(std::max(largest_layer_, largest_domain_)),
      leaders_1_(largest_layer_),
      leaders_2_(largest_layer_),
      layer_ids_1_(largest_layer_),
      layer_ids_2_(largest_layer_),
      layer_nodes_(largest_layer_),
      layer_arcs_(largest_layer_),
      layer_values_(largest_layer_),
      raddix_count_(radix_size_),
      raddix_cumulative_(radix_size_),
      escape_node_(-1, largest_layer_ - 1),
      escape_edge_(&escape_node_, &escape_node_, largest_domain_ - 1) {
  non_null_raddix_.reserve(radix_size_);
  escape_node_.info1_ = largest_layer_ - 1;
  if (mdd.Final() ==
      nullptr) {  // If the MDD is empty, nothing needs to be done
    return;
  }
  mdd.Final()->info1_ = 0;
  for (int layer = mdd.getSize() - 1; layer >= 0;
       layer--) {  // update of the arcs (bottom up)

    currentLayerSize = ArcsUpdateRadixInit(layer);
    // Optim: all the arcs from last layer terminate on the same node ?
    raddix_mode = true;
    swap_status = true;
    over = escape_edge_.Value();
    while (currentLayerSize > 1) {
      RadixCount(LayerID());
      // # of final pack (either alone or equiv)
      delta = raddix_count_[over];
      RadixCumulative();
      Permutation(LayerID(), LayerIDNext());

      swap_status = !swap_status;  // SWAP

      LeadManagement(Leader(), LeaderNext(), LayerID(), LayerIDNext());

      currentLayerSize -= delta;    // remove terminated packs
      if (currentLayerSize == 0) {  // layer done
        break;
      }
      if (raddix_mode) {
        PrepareNextRadixLabel();
      } else {
        PrepareNextRadixNode();
      }
      raddix_mode = !raddix_mode;
    }
    Merge(LayerID(), Leader());
  }

  mdd.clearDel();
}

int Reduce::ArcsUpdateRadixInit(int layer) {
  layer_size = 0;
  n = mdd_.getNodeLvl(layer);
  while (n) {
    n_next = n->next_;
    e = n->Arcs();
    n->newV_ = n;
    while (e) {  // Extract info from layer n+1 already processed
      e_next = e->next_;
      n->updateArc(e, e->end_->newV_);
      e = e_next;
    }
    if (n->Arcs()) {
      layer_nodes_[layer_size] = n;
      layer_arcs_[layer_size] = n->Arcs();
      layer_values_[layer_size] = n->Arcs()->value_;
      layer_ids_1_[layer_size] = layer_size;
      layer_ids_2_[layer_size] = layer_size;
      leaders_1_.setLeader(layer_size, 0);
      leaders_2_.setLeader(layer_size, 0);

      n->info1_ = layer_size;  // used for the raddix
      layer_size++;
    } else {
      mdd_.removeList(n);
      n->newV_ = nullptr;  // nodes of layer above needs to know the node is out
    }
    n = n_next;
  }
  return layer_size;
}

void Reduce::RadixCount(std::vector<int>& l_id) {
  for (int n_id = 0; n_id < currentLayerSize; n_id++) {
    const int e_val = layer_values_[l_id[n_id]];
    if (raddix_count_[e_val]++ == 0) {
      non_null_raddix_.push_back(e_val);  // Only need of only non empty
    }
  }
}

void Reduce::RadixCumulative() {
  id = non_null_raddix_[0];
  if (id == over) {
    id = non_null_raddix_.back();
    non_null_raddix_[0] = non_null_raddix_.back();
    non_null_raddix_.back() = over;
  }
  raddix_cumulative_[id] = 0;
  for (size_t prev_n_id = 1; prev_n_id < non_null_raddix_.size();
       prev_n_id++) {  // cumulative
    id = non_null_raddix_[prev_n_id];
    if (id == over) {
      id = non_null_raddix_.back();
      non_null_raddix_[prev_n_id] = non_null_raddix_.back();
      non_null_raddix_.back() = over;
    }
    id_prev = non_null_raddix_[prev_n_id - 1];
    raddix_cumulative_[id] =
        raddix_cumulative_[id_prev] + raddix_count_[id_prev];
    raddix_count_[id_prev] = 0;
  }
  raddix_count_[non_null_raddix_.back()] = 0;
  non_null_raddix_.clear();
}

void Reduce::Permutation(std::vector<int>& l_id_1, std::vector<int>& l_id_2) {
  for (int n_id = 0; n_id < currentLayerSize; n_id++) {
    l_id_2[raddix_cumulative_[layer_values_[l_id_1[n_id]]]++] = l_id_1[n_id];
  }
}

void Reduce::LeadManagement(unionFind& lead_1, unionFind& lead_2,
                            std::vector<int>& l_id_1,
                            std::vector<int>& l_id_2) {
  lead_1.setLeader(l_id_1[0], l_id_1[0]);

  for (int n_id = 1; n_id < currentLayerSize; n_id++) {  // Lead managing
    id = l_id_1[n_id];
    id_prev = l_id_1[n_id - 1];
    lead_1.setLeader(id, id);
    if (layer_values_[id] == layer_values_[id_prev]) {    // Same val
      if (lead_2.leader(id) == lead_2.leader(id_prev)) {  // same pack
        lead_1.setLeader(id, lead_1.leader(id_prev));
      }
    }
  }

  for (int n_id = currentLayerSize - delta; n_id < currentLayerSize;
       n_id++) {  // avoid missing nodes after the shrink
    l_id_2[n_id] = l_id_1[n_id];
    lead_2.setLeader(l_id_1[n_id], lead_1.leader(l_id_2[n_id]));
  }
}

void Reduce::PrepareNextRadixLabel() {
  for (int n_id = 1; n_id < currentLayerSize;
       n_id++) {  // manage pack of size 1
    id = layer_ids_2_[n_id];
    id_prev = layer_ids_2_[n_id - 1];
    if (leaders_2_.leader(id) != leaders_2_.leader(id_prev) &&
        id_prev == leaders_2_.leader(id_prev)) {  // node id_prev is unique
      layer_arcs_[id_prev] = &escape_edge_;
    }
    layer_values_[id_prev] = layer_arcs_[id_prev]->end_->info1_;
  }
  id = layer_ids_2_[currentLayerSize - 1];
  if (leaders_2_.leader(id) == id) {
    layer_arcs_[id] = &escape_edge_;
  }
  layer_values_[id] = layer_arcs_[id]->end_->info1_;

  over = escape_node_.info1_;
}

void Reduce::PrepareNextRadixNode() {
  id = layer_ids_1_[0];
  layer_arcs_[id] = (layer_arcs_[id]->next_ == nullptr)
                        ? &escape_edge_
                        : layer_arcs_[id]->next_;
  for (int n_id = 1; n_id < currentLayerSize;
       n_id++) {  // manage pack of size 1
    id = layer_ids_1_[n_id];
    id_prev = layer_ids_1_[n_id - 1];
    layer_arcs_[id] = (layer_arcs_[id]->next_ == nullptr)
                          ? &escape_edge_
                          : layer_arcs_[id]->next_;
    if (leaders_1_.leader(id) != leaders_1_.leader(id_prev) &&
        id_prev == leaders_1_.leader(id_prev)) {  // node id_prev is unique
      layer_arcs_[id_prev] = &escape_edge_;
    }
    layer_values_[id_prev] = layer_arcs_[id_prev]->value_;
  }
  id = layer_ids_1_[currentLayerSize - 1];
  if (leaders_1_.leader(id) == id) {
    layer_arcs_[id] = &escape_edge_;
  }
  layer_values_[id] = layer_arcs_[id]->value_;

  over = escape_edge_.Value();
}

void Reduce::Merge(std::vector<int>& layer_id, unionFind& leader) {
  for (int n_id = 0; n_id < layer_size; n_id++) {  // Merge equivalent nodes
    id = layer_id[n_id];
    layer_nodes_[id]->newV_ = layer_nodes_[leader.leader(id)]->newV_;
    if (id != leader.leader(id)) {
      mdd_.removeList(layer_nodes_[id]);
    }
  }
}

}  // namespace MDD
