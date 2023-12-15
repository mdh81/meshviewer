# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)


foreach (test ${allTests})
    add_executable(${test} ${test}.cpp)
    target_include_directories(
        ${test} 
        PUBLIC
        ../
    )
    # All tests link against gtest, events, and gl libraries
    target_link_libraries(
        ${test}
        PUBLIC
        ${glLibs}
        "-framework OpenGL"
        gtest_main
        events
    )  
    gtest_discover_tests(${test})
endforeach(test)