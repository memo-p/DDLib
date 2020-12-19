PROJECT(Bench)

# All sources that also need to be tested in unit tests go into a static library
set(APP_HEADERS
        src/MISP/RelaxIndependentSet.hpp
)

set(APP_SOURCES
        src/main.cpp
        )

        
set(APPLICATION_NAME "bench")
add_executable(${APPLICATION_NAME} ${APP_SOURCES} ${APP_HEADERS})
target_link_libraries(${APPLICATION_NAME} MDD cxxopts json-nlohmann)

target_compile_options(${APPLICATION_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})
