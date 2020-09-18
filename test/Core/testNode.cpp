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

#define CATCH_CONFIG_MAIN  

#include "catch.hpp"
#include "Core/MDD.hpp"

TEST_CASE("test basic creation") {
    MDD::Node n(5,8);

    CHECK(n.VarIndex() == 5);
    CHECK(n.UID() == 8);
    CHECK(n.Arcs() == nullptr);
    CHECK(n.lastArc() == nullptr);
    CHECK(n.get(15) == nullptr);
    CHECK(n.InDegree() == 0);
    CHECK(n.OutDegree() == 0);
} 


TEST_CASE("test 2 creation + 1 arc") {
    MDD::Node n1(0,1);
    MDD::Node n2(1,2);

    auto a = n1.AddArc(12, &n2);

    CHECK(n1.Arcs() == a);
    CHECK(n1.lastArc() == a);
    CHECK(n1.get(15) == nullptr);
    CHECK(n1.get(12) == a);
    CHECK(n1.InDegree() == 0);
    CHECK(n1.OutDegree() == 1);

    CHECK(n2.Arcs() == nullptr);
    CHECK(n2.lastArc() == nullptr);
    CHECK(n2.get(15) == nullptr);
    CHECK(n2.InDegree() == 1);
    CHECK(n2.OutDegree() == 0);
} 


TEST_CASE("test creation MDD") {
    MDD::Node n1(0,1);
    MDD::Node n2(1,2);

    auto a = n1.AddArc(12, &n2);

    CHECK(n1.Arcs() == a);
    CHECK(n1.lastArc() == a);
    CHECK(n1.get(15) == nullptr);
    CHECK(n1.get(12) == a);
    CHECK(n1.InDegree() == 0);
    CHECK(n1.OutDegree() == 1);

    CHECK(n2.Arcs() == nullptr);
    CHECK(n2.lastArc() == nullptr);
    CHECK(n2.get(15) == nullptr);
    CHECK(n2.InDegree() == 1);
    CHECK(n2.OutDegree() == 0);
} 
