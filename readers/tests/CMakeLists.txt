add_executable(
    stlreader_test 
    stlreader_test.cpp)

# Convenience variables
set(allTests stlreader_test)

# All tests include headers from the source directory
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../mesh
    )
endforeach(test)

# All tests link against gtest, reader, mesh and common libraries 
foreach(test ${allTests})
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        readers 
        mesh
        common
    )  
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(test)
