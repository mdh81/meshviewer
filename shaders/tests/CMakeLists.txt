# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)

# All tests include headers from the source directory
foreach(test ${allTests})
    add_executable(${test} ${test}.cpp)

    target_include_directories(
        ${test} 
        PUBLIC
        ../
    )
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        shaders
        mesh
        events
        common
        ${glLibs}
        "-framework OpenGL"
    )
    gtest_discover_tests(${test})
endforeach(test)