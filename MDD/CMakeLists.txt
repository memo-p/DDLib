project(MDD)

set(SOURCE
Core/MDD.cpp
)
set(HEADERS
Core/MDD.hpp
DataStructures/list.hpp
DataStructures/orderedList.hpp
)

set(LIB_NAME "MDD")
add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADERS})

target_link_libraries(${LIB_NAME})

target_compile_options(${LIB_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})


# Set two minimum target properties for the library.
# See https://cmake.org/cmake/help/latest/command/set_target_properties.html
# Properties are: 1) Library version number 2) list of corresponding public headers
set_target_properties(${LIB_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


