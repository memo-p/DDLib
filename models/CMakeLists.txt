project(DP-models)

set(HEADERS
AllDifferent.hpp
DynSum.hpp
IndepSet.hpp
SetCover.hpp
Partitioners/setPartitioner.hpp
)

set(LIB_NAME "DP-models")
add_library(${LIB_NAME} STATIC ${HEADERS})

target_link_libraries(${LIB_NAME} MDD)

target_compile_options(${LIB_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})


# Set two minimum target properties for the library.
# See https://cmake.org/cmake/help/latest/command/set_target_properties.html
# Properties are: 1) Library version number 2) list of corresponding public headers
set_target_properties(${LIB_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


