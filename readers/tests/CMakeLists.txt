add_executable(
    reader_factory_test
    reader_factory_test.cpp)

add_executable(
    stl_reader_test
    stl_reader_test.cpp)

# Convenience variables
set(allTests reader_factory_test stl_reader_test)

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
# Makes no sense to link against shaders for testing. Create mock objects
foreach(test ${allTests})
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        readers 
        mesh
        common
        shaders
        objects
        events
    )  
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(test)
