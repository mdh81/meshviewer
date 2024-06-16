# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)

foreach (test ${allTests})
    add_executable(${test} ${test}.cpp)
    # Include directories directly instead of relying on target_link_libraries to bring in include paths.
    # This prevent tests from becoming dependent on other libraries
    target_include_directories(
        ${test}
        PUBLIC
        ../../mesh
        ../../common/testsupport
        ../../objects/
        ../../common/
        # TODO: Remove this by excluding objects from the include path
        ${glIncludeDirectories}
    )

    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        gmock
        readers
    )

    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(test)
