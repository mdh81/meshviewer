add_executable(
    object_test 
    object_test.cpp)

# Convenience variables
set(allTests object_test)

# All tests include headers from the source directory and from mesh 
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../mesh
    )
endforeach(test)

# All tests link against gtest, common, and mesh 
foreach(test ${allTests})
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        common 
        mesh
        objects
        shaders
        events
    )  
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test})
endforeach(test)
