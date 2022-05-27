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

#ifndef MDD_BUILDERS_BASE
#define MDD_BUILDERS_BASE

#include <stdio.h>
#include "Core/MDD.hpp"

namespace MDD {

class MDDBuilder {
public:
    
    virtual ~MDDBuilder() = default;
    
    virtual MDD * Build() = 0;
    
};

class EmptyBuilder: public MDDBuilder {
    int vars_;
    int domSize_;
public:
    EmptyBuilder(int vars, int domSize):
        vars_(vars),
        domSize_(domSize)
        {}
    
    MDD * Build(){
        MDD * mdd = new MDD(vars_);
        mdd->setDomSize(domSize_);
        mdd->createNode(0);
        mdd->createNode(vars_);
        return  mdd;
    }
    
};

}  // namespace MDD

#endif /* MDD_BUILDERS_BASE */
