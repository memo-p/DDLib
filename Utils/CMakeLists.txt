project(MDD-Utils)

set(SOURCE
Builders/Automaton.cpp
Builders/DynProg.cpp
Builders/Transitions.cpp
Builders/Table.cpp
Builders/Grid.cpp
Core/Triplet.cpp
DataStructures/BitSets.cpp
DataStructures/TableBuilder.cpp
DataStructures/TableSort.cpp
Operations/Apply.cpp
Operations/Reduce.cpp
Utils/ChronoP.cpp
Utils/MDDInfos.cpp
)
set(HEADERS
mdds.hpp
Builders/base.hpp
Builders/Automaton.hpp
Builders/DynProg.hpp
Builders/Transitions.hpp
Builders/Table.hpp
Builders/Grid.hpp
Core/Triplet.hpp
DataStructures/BitSets.hpp
DataStructures/SparseSet.hpp
DataStructures/Table.hpp
DataStructures/TableBuilder.hpp
DataStructures/TableSort.hpp
DynamicProg/ApplyDP.hpp
DynamicProg/DynP.hpp
Operations/Apply.hpp
Operations/Reduce.hpp
Operations/ReduceDFSMap.hpp
Operations/Search.hpp
Utils/ChronoP.hpp
Utils/MDDInfos.hpp
Utils/union.hpp
Relax/Creation/DPRelaxCreation.hpp
Relax/Partitioners/StatePartitioner.hpp
Relax/Reduction/reduce.hpp
Relax/Partitioner.hpp
Relax/State.hpp
Relax/Utils.hpp
)

set(LIB_NAME "MDD-Utils")
add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADERS})

target_link_libraries(${LIB_NAME} MDD infInt)

target_compile_options(${LIB_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})


# Set two minimum target properties for the library.
# See https://cmake.org/cmake/help/latest/command/set_target_properties.html
# Properties are: 1) Library version number 2) list of corresponding public headers
set_target_properties(${LIB_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


