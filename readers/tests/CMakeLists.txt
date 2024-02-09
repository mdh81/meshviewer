# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)

foreach (test ${allTests})
    add_executable(${test} ${test}.cpp)
    target_include_directories(
        ${test}
        PUBLIC
        ../
        ../../mesh
    )
    # TODO: Create mocks for all dependencies. Reader tests should link only against reader library
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        readers
        mesh
        common
        objects
        events
        ${glLibs}
        shaders
    )
    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(test)
