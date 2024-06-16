# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)

# NOTE: Tests don't link against models directly to prevent the tests from being dependent on
# model's dependencies like mesh, which in turn depend on the rendering layer
file(GLOB modelSources ../*.cpp)
foreach (test ${allTests})
    add_executable(${test} ${test}.cpp ${modelSources})
    target_link_libraries(
        ${test}
        PRIVATE
        gtest_main
        gmock
        events
        common
    )

    # Include headers of dependencies directly
    target_include_directories(
        ${test}
        PRIVATE
        ../../readers
        ../../mesh
        ../../objects
        ../../common/testsupport
        ../../models
        ../../common
        ../../viewer
        ${glIncludeDirectories}
    )

    # Add test to ctest discovery
    gtest_discover_tests(${test})

endforeach(test)
