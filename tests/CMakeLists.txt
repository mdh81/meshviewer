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
set(viewerTests viewer_test shaderloader_test camera_factory_test)
set(nonViewerTests mesh_test stlreader_test octree_test object_test)

# All tests include headers from the source directory 
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
    )
endforeach(test)

# Non-viewer tests link against gtest and meshviewer
foreach(test ${nonViewerTests})
    target_link_libraries(
        ${test}
        gtest_main
        meshviewer
    )  
endforeach(test)

# Viewer tests link against GL, meshviwer, events, and gtest
foreach(test ${viewerTests})
    target_link_directories(
        ${test}
        PUBLIC
        ${glLibDirs}
    )
    target_link_libraries(
        ${test} 
        PUBLIC
        gtest_main
        meshviewer
        events
        ${glLibs}
        "-framework OpenGL"
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
