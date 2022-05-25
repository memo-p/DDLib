PROJECT(Applications)

function(create_app  APPLICATION_NAME APP_SOURCE LIBS)
    add_executable(${APPLICATION_NAME} ${APP_SOURCE} ${APP_HEADERS})
    target_link_libraries(${APPLICATION_NAME} ${LIBS})
    target_compile_options(${APPLICATION_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})
    set_target_properties(${APPLICATION_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/apps)
endfunction(create_app)

set(LIBS_APPS MDD DP-models DP-benchmarks cxxopts json-nlohmann)

# The apps program
create_app(MISP src/MISP.cpp "${LIBS_APPS}")
create_app(AllDifferent src/AllDifferent.cpp "${LIBS_APPS}")
create_app(SetCover src/SetCover.cpp "${LIBS_APPS}")
