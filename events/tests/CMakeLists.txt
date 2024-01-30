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
    # All tests link against gtest and events
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        events
    )  
    gtest_discover_tests(${test})
endforeach(test)