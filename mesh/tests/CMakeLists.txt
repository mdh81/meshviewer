# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)

foreach (test ${allTests})
    add_executable(${test} ${test}.cpp)

    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../readers
    )

    # TODO: Use mocks and replace needless test dependencies
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        events
        mesh
        common
        objects
        shaders
        readers
        ${glLibs}
    )

    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")

endforeach(test)