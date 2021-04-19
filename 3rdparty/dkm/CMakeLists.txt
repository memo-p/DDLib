project (dkm)

# Header only library, therefore INTERFACE
add_library(dkm INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(dkm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
