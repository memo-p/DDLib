project (infInt)

# Header only library, therefore INTERFACE
add_library(infInt INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(infInt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
