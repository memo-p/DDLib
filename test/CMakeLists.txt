project(solver_tests)

function(create_unit_test  test_name test_location LIBS)
    add_executable(${test_name} ${test_location})
    target_compile_options(${test_name} PRIVATE ${CXX_FLAGS_REQUIRED_TESTS})
    target_include_directories(${test_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${test_name} ${LIBS}  )
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
endfunction(create_unit_test)




set(LIBS_TEST MDD catch DP-models DP-benchmarks)


# The test program
create_unit_test(testbuilders Builder/testBuilder.cpp "${LIBS_TEST}")
create_unit_test(testDPbuilders Builder/testDPBuild.cpp "${LIBS_TEST}")
create_unit_test(testLists DataStructures/testLists.cpp "${LIBS_TEST}")
create_unit_test(testMDDInfos Utils/testMDDInfos.cpp "${LIBS_TEST}")
create_unit_test(testNode Core/testNode.cpp "${LIBS_TEST}")
create_unit_test(testApply Operations/testApply.cpp "${LIBS_TEST}")
create_unit_test(testReduce Operations/testReduce.cpp "${LIBS_TEST}")
create_unit_test(testTable DataStructures/testTable.cpp "${LIBS_TEST}")
create_unit_test(testRelaxDPC Relax/testRelaxDPCreation.cpp "${LIBS_TEST}")
create_unit_test(testRelaxReduce Relax/testRelaxReduce.cpp "${LIBS_TEST}")
create_unit_test(testRestrictedDPC Restricted/testRestrictedDPCreation.cpp "${LIBS_TEST}")

###BUILD TEST Examples
set(LIBS_EXAMPLES  ${LIBS_TEST})

