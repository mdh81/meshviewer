add_executable(
    glyph_test 
    glyph_test.cpp)

# Add test coverage for gradient background

# Convenience variables
set(allTests glyph_test)

# All tests include headers from the source directory
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
    )
endforeach(test)

# All tests link against gtest
foreach(test ${allTests})
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        objects
        common
        mesh
        events
    )
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(test)
