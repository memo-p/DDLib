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

#ifndef SRC_OPERATIONS_SEARCH
#define SRC_OPERATIONS_SEARCH

#include <stdio.h>
#include <functional>

#include <queue>
#include <unordered_map>

#include "Core/MDD.hpp"

namespace MDD {

/**
 * Base class to do a DFS on the MDD.
 **/
class DepthFirstSearch {
 private:
  MDD &mdd_;
  std::unordered_map<Node *, bool> visited_;

 public:
  DepthFirstSearch(MDD &mdd) : mdd_(mdd) {}
  virtual ~DepthFirstSearch() {}
  /**
   * Run DFS on the MDD.
   **/
  void Run() {
    OnStart();
    ExploreNode(GetMDD().Root());
    OnStop();
  }

  MDD &GetMDD() { return mdd_; }
  /**
   * Method called when a node starts being processed.
   **/
  virtual void OnOpenNode(Node *n) { (void)n; }
  /**
   * Method called when a node stops being processed.
   **/
  virtual void OnCloseNode(Node *n) { (void)n; }
  /**
   * Method called when an arc is visited.
   * Before the ending node is explored.
   **/
  virtual void OnOpenArc(Arc *a) { (void)a; }
  /**
   * Method called when an arc is visited.
   * After the ending node is explored.
   **/
  virtual void OnCloseArc(Arc *a) { (void)a; }

  /**
   * Method called before the search starts.
   **/
  virtual void OnStart() {}
  /**
   * Method called After the search ends.
   **/
  virtual void OnStop() {}

 private:
  void ExploreNode(Node *n) {
    visited_[n] = true;
    OnOpenNode(n);
    Arc *a = n->Arcs();
    Arc *a_next = nullptr;
    while (a) {
      OnOpenArc(a);
      if (!visited_[a->End()]) {
        ExploreNode(a->End());
      }
      a_next = a->Next();
      OnCloseArc(a);
      a = a_next;
    }
    OnCloseNode(n);
  }
};

/**
 * Base class to do a BFS on the MDD.
 **/
class BreadthFirstSearch {
 private:
  MDD &mdd_;
  std::unordered_map<Node *, bool> visited_;
  std::queue<Node *> nodes_;

 public:
  BreadthFirstSearch(MDD &mdd) : mdd_(mdd) {}
  virtual ~BreadthFirstSearch() {}
  /**
   * Run BFS on the MDD.
   **/
  void Run() {
    OnStart();
    nodes_.push(GetMDD().Root());
    while (nodes_.size()) {
      Node *n = nodes_.front();
      nodes_.pop();
      OnOpenNode(n);
      Arc *a = n->Arcs();
      while (a) {
        OnArc(a);
        if (!visited_[a->End()]) {
          visited_[a->End()] = true;
          nodes_.push(a->End());
        }
        a = a->Next();
      }
      OnCloseNode(n);
    }
    OnStop();
  }

  MDD &GetMDD() { return mdd_; }
  /**
   * Method called when a node starts being processed.
   **/
  virtual void OnOpenNode(Node *n) { (void)n; }
  /**
   * Method called when a node stops being processed.
   **/
  virtual void OnCloseNode(Node *n) { (void)n; }
  /**
   * Method called when an arc is visited.
   **/
  virtual void OnArc(Arc *a) { (void)a; }
  /**
   * Method called before the search starts.
   **/
  virtual void OnStart() {}
  /**
   * Method called After the search ends.
   **/
  virtual void OnStop() {}
};

/**
 * Base class to do a BFS on the MDD.
 **/
class BreadthFirstSearchCallback : public BreadthFirstSearch {
 private:
  std::function<void(Node *)> on_open_node_;
  std::function<void(Node *)> on_close_node_;
  std::function<void(Arc *)> on_arc_;
  std::function<void()> on_start_;
  std::function<void()> on_stop_;

 public:
  BreadthFirstSearchCallback(MDD &mdd, std::function<void(Node *)> const &on_open_node,
                             std::function<void(Node *)> const &on_close_node,
                             std::function<void(Arc *)> const &on_arc,
                             std::function<void()> const &on_start,
                             std::function<void()> const &on_stop)
      : BreadthFirstSearch(mdd),
        on_open_node_(on_open_node),
        on_close_node_(on_close_node),
        on_arc_(on_arc),
        on_start_(on_start),
        on_stop_(on_stop) {}

  BreadthFirstSearchCallback(MDD &mdd)
      : BreadthFirstSearchCallback(
            mdd, [](Node *) {}, [](Node *) {}, [](Arc *) {}, []() {}, []() {}) {}

  void SetOnOpenNode(std::function<void(Node *)> const &on_open_node) {
    on_open_node_ = on_open_node;
  }
  void SetOnCloseNode(std::function<void(Node *)> const &on_close_node) {
    on_close_node_ = on_close_node;
  }
  void SetOnArc(std::function<void(Arc *)> const &on_arc) { on_arc_ = on_arc; }
  void SetOnStart(std::function<void()> const &on_start) { on_start_ = on_start; }
  void SetOnStop(std::function<void()> const &on_stop) { on_stop_ = on_stop; }

  void OnOpenNode(Node *n) override { on_open_node_(n); }
  void OnCloseNode(Node *n) override { on_close_node_(n); }
  void OnArc(Arc *a) override { on_arc_(a); }
  void OnStart() override { on_start_(); }
  void OnStop() override { on_stop_(); }
};

}  // namespace MDD

#endif /* SRC_OPERATIONS_SEARCH */
