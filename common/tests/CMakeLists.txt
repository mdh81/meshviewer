# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)


foreach (test ${allTests})
    add_executable(${test} ${test}.cpp)
    # All tests include headers from the source directory and from mesh
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../mesh
    )
    # TODO: Create mocks for all dependencies and remove reliance on glLibs
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        common
        mesh
        objects
        shaders
        events
        ${glLibs}
    )
    gtest_discover_tests(${test})
endforeach(test)