# MDD
This library is a simple implementation of modeling algorithms using Decision Diagrams (MDDs).

## Use
The main class of this Library is _MDD_. 
A MDD contains _Node_ objects and _Node_ Objects contain _Arc_ objects.
They are the main, and only, different objects that any user need to be aware of.

### Creation
Obtaining an MDD can be done using either a table (i.e. a set of tuples) (_MDDBuilderFromTable_) ,
using an automaton (_MDDBuilderAutomaton_), using some predefined MDD (_MDDBuilderFromTransition_),
or using the grid automatic construction (_MDDBuilderGrid_).

Here is an example of creation using a table of tuples.
```C++
  TableOfTuple table(3, {2, 3, 2});
  std::vector<std::vector<int>> tuples = {
      {1, 1, 1}, {0, 0, 1}, {0, 1, 0}, {0, 0, 0}};
  table.addTuples(tuples);

  MDDBuilderFromTable bt(table);
  MDD::MDD* mdd = bt.Build();
```

Finally, you can build your own MDDs using the node and arc creation API:
```C++
 MDD::Node n1(0,1);
 MDD::Node n2(1,2);
 auto a = n1.AddArc(12, &n2);
```

### Operations
A classic usage of MDD is to combine them together. Most set operations are available using MDDs.
For example, performing the intersection between two MDDs can be done as follow:
```C++
 MDD::MDD* mdd1 = bt1.Build();
 MDD::MDD* mdd2 = bt2.Build();
 Apply inter(*mdd1, *mdd2, Apply::op_intersection);
 MDD::MDD* mdd_inter = inter.Run();
```

The different operations easily accessible are : 
```C++
Apply::op_intersection
Apply::op_union
Apply::op_difference 
Apply::op_xor 
Apply::op_not_union
Apply::op_not_intersection
```

For other operations, you can build them using the different construction flags:
```C++
const unsigned int Apply::op_not_not_all_ = 1u;
const unsigned int Apply::op_not_yes_all_ = 2u;
...
const unsigned int Apply::op_yes_yes_last_ = 128u;
```

As showned in 
> Guillaume Perez, Jean-Charles Régin. Efficient Operations On MDDs For Building Constraint Programming Models. IJCAI 2015.


### Reduction
The last most important operation for MDDs is the _reduction_. 
It takes an MDD in which some nodes are equivalent (i.e. they contains the exact same set of labeled sub-paths), 
and merge them together.

This can be simply done as follow:

```C++
Reduce r_p(*mdd);
```


### Relax MDDs
This library propose an implementation of the relax algorithms defined in 
> Guillaume Perez. Decision diagrams: constraints and algorithms (PhD thesis 2017)

In this implementation of the relax MDD, two criteria are used instead of one.
The first one is analogous of known relax MDDs, it is the width. 
The width is the maximum number of nodes that the MDD can have in each layer.
The second criterion is the depth. 
The depth is the size of the sub-tree that will be used and developed during the relax merging process.

#### Relax reduction
The relax reduction consists in relaxing the criteria used by the classic reduction.
Instead of equivalent nodes, a heuristic function will choose which nodes need to be merged.
When a layer of the MDD becomes larger than the width criteria, a partitioning function is used to select
the nodes that should be merged together.
Some example of Partitioning function can be find in _src/Relax/Partitioner.hpp_.

Given a MDD _mdd_, this operation is coded as follow:
```C++
  Partitioner p;
  RelaxReduce(*mdd, 1, &p);
```
The only difference with basic reduction is the partitioning function.

#### Relax Creation of dynamic programs
The relax creation rely on two components. 
The first one is the definition of the dynamic program, this is done by the _DynamicProgram_ class.
Two examples are given in _src/DynamicProg/IndepSet.hpp_ and _src/DynamicProg/DynSum.hpp_ for respectively, the 
independent set problem in graph theory and the dynamic sum processing.

The second component is the partitioning function used by the relax reduction.

A working example of how to use the relax creation for a relax sum problem is :
```C++
 int nb_vars = 3;
  int nb_values = 2;  // 0, 1
  int max_width = 2;
  int max_depth = 1;
  int max_DPSum = 2;
  SumDynProg sdp(max_DPSum);
  Partitioner p;
  DynamicProgRelaxCreation dprc(nb_vars, nb_values, &sdp, &p, max_width,
                                max_depth);
```

An example for the independent set can be found in _benchs/src/RelaxIndependentSet.hpp_


#### Relax Operations
In the same way as the reduction operation can be relaxed, the apply operation can be too.

The current implementation (_src/DynamicProg/ApplyDP.hpp_) uses the dynamic
programming relax constructor, and can use different _Partitioner_ objects. 

The following example for the intersection could be define:
```C++
 int width = 40;
 int depth = 2;
 MDD::MDD* mdd1 = bt1.Build();
 MDD::MDD* mdd2 = bt2.Build();
 ApplyDP inter(*mdd1, *mdd2, Apply::op_intersection);
 Partitioner p;
 DynamicProgRelaxCreation dp_ap(mdd1->getSize(), domain, &inter, &p, width,
                                 depth);
 MDD* mdd = dp_ap.Build();
```

## 3rd party
This Library is made to be plugged as a 3rd party library.
For constraint solver for example.
All the non-lib compilation is made optional by default.

## Local compilation and test
```
rm -rf Debug ; mkdir Debug ; cd Debug ; cmake -GNinja -DALL_MDD=ON -DCMAKE_BUILD_TYPE=Debug .. ; ninja
```

```
rm -rf Release ; mkdir Release ; cd Release ; cmake -GNinja -DALL_MDD=ON -DCMAKE_BUILD_TYPE=Release .. ; ninja
```

## Licence

This project is a multi-licence project. The DDLib has an MIT-licence, and each 3rdparty have it's own licence. 
They are simply provided as is, and any information about their licence is in their directory.

- catch : Boost Software License, Version 1.0
- InfInt : Mozilla Public License, v. 2.0
- cxxopt : MIT-licence
- nlohmann-json : MIT licence