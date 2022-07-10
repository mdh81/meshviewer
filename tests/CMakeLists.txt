add_executable(
    mesh_test
    mesh_test.cpp)

add_executable(
    stlreader_test
    stlreader_test.cpp
)
add_executable(
    octree_test
    octree_test.cpp
)

add_executable(
    viewer_test
    viewer_test.cpp
)

add_executable(
    shaderloader_test 
    shaderloader_test.cpp
)

add_executable(
    object_test
    object_test.cpp
)

add_executable(
    camera_factory_test
    camera_factory_test.cpp
)

# Convenience variables
set(allTests mesh_test stlreader_test octree_test viewer_test shaderloader_test object_test camera_factory_test)
set(nonViewerTests mesh_test stlreader_test octree_test object_test camera_factory_test)

# All tests (viewer and non-viewer) link against gtest and meshviewer
foreach(test ${nonViewerTests})
    target_link_libraries(
        ${test}
        gtest_main
        meshviewer
    )  
endforeach(test)

# Viewer test links against GL libraries
target_link_directories(
    viewer_test 
    PUBLIC
    ${glLibDirs}
)
target_link_libraries(
    viewer_test
    PUBLIC
    gtest_main
    meshviewer
    events
    ${glLibs}
    "-framework OpenGL"
)

# Shader loader test links against GL libraries
target_link_directories(
    shaderloader_test 
    PUBLIC
    ${glLibDirs}
)
target_link_libraries(
    shaderloader_test 
    PUBLIC
    gtest_main
    meshviewer
    ${glLibs}
)
target_link_libraries(
    shaderloader_test 
    PUBLIC
    "-framework OpenGL"
)
# Include GL headers for shader test
target_include_directories(
    shaderloader_test
    PUBLIC
    /usr/local/include
)

# All tests include headers from the source directory 
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
    )
endforeach(test)

file(
    COPY
    testfiles
    DESTINATION 
    ${CMAKE_BINARY_DIR}/tests
)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test})
endforeach(test)
