project (json-nlohmann)

# Header only library, therefore INTERFACE
add_library(json-nlohmann INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(json-nlohmann INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/json.hpp DESTINATION include )

install(TARGETS json-nlohmann
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib/static COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)