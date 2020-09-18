cmake_minimum_required(VERSION 3.1)

# The project name
project(mdd_lib)

#### COMPILER OPTIONS 
set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS_REQUIRED -Wall -Wextra)

if(DCMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CXX_FLAGS_REQUIRED ${CXX_FLAGS_REQUIRED} -g2)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(SYSTEM "3rdparty/InfInt/include")
add_subdirectory(3rdparty/InfInt)
add_subdirectory(3rdparty/cxxopts-2.2.0)
add_subdirectory(3rdparty/nlohmann-json-3.7.3)
add_subdirectory(src)


add_subdirectory(benchs)

include(CTest)
add_subdirectory(3rdparty/catch)
add_subdirectory(test)

