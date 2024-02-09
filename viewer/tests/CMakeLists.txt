# Rely on common/testsupport/TestSupport.cmake to populate allTests
addTestSources()

include(GoogleTest)

foreach(test ${allTests})

    add_executable(${test} ${test}.cpp)

    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../common
        ../../mesh
        ../../common/testsupport
        ../../objects
    )

    target_link_libraries(
        ${test} 
        PUBLIC
        gtest_main
        common
        viewer
        events
        mesh
        objects
        configuration
        gmock
        ${glLibs}
        shaders
    )
    gtest_discover_tests(${test})
endforeach(test)
