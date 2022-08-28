add_executable(
    mesh_test 
    mesh_test.cpp)

add_executable(
    octree_test
    octree_test.cpp)

# Convenience variables
set(allTests mesh_test octree_test)

# All tests include headers from the source directory
# mesh_test depends on glm, so add glIncDirs
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../readers
        ${glIncDirs}
    )
endforeach(test)

# All tests link against gtest, mesh, reader and common libraries 
foreach(test ${allTests})
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        events
        mesh
        common
        readers
    )  
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(test)
