cmake_minimum_required(VERSION 3.1)

# The project name
project(mdd_lib)

#### COMPILER OPTIONS 
set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS_REQUIRED -Wall -Wextra -pedantic-errors -Wconversion)

if(DCMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CXX_FLAGS_REQUIRED ${CXX_FLAGS_REQUIRED} -g2)
endif()



#####CMAKE OPTIONS
option(ALL_MDD "ALL_MDD" OFF)
option(APPS_DD "APPS_MDD" OFF)
option(TESTS_MDD "TESTS_MDD" OFF)

if(ALL_MDD)
  set(APPS_DD ON)
  set(TESTS_MDD ON)
endif()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT TARGET InfInt)
  include_directories(SYSTEM "3rdparty/InfInt/include")
  add_subdirectory(3rdparty/InfInt)
endif() 

if (NOT TARGET cxxopts)
  include_directories(SYSTEM "3rdparty/cxxopts-2.2.0/include")
  add_subdirectory(3rdparty/cxxopts-2.2.0)
endif() 
if (NOT TARGET json-nlohmann)
  add_subdirectory(3rdparty/nlohmann-json-3.7.3)
endif() 
if (NOT TARGET dkm)
  include_directories(SYSTEM "3rdparty/dkm/include")
  add_subdirectory(3rdparty/dkm)
endif() 

add_subdirectory(MDD)
add_subdirectory(models)
add_subdirectory(benchmarks)

if(APPS_DD)
  add_subdirectory(apps)
endif()

if(TESTS_MDD)
  include(CTest)
  if (NOT TARGET catch)
    add_subdirectory(3rdparty/catch)
  endif() 
  add_subdirectory(test)
endif()

if (NOT TARGET dkm)
  include_directories(SYSTEM "3rdparty/dkm/include")
  add_subdirectory(3rdparty/dkm)
endif() 

