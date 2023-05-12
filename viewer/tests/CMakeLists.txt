add_executable(
    viewer_test
    viewer_test.cpp
)

add_executable(
    scene_test
    scene_test.cpp
)

add_executable(
    viewport_test
    viewport_test.cpp
)

set(allTests viewer_test scene_test viewport_test)

foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../common
        ../../mesh
        ../../common/testsupport
        ../../objects
    )
endforeach(test)

foreach(test ${allTests})
    target_link_libraries(
        ${test} 
        PUBLIC
        gtest_main
        ${glLibs}
        "-framework OpenGL"
        common
        viewer
        shaders
        events
        mesh
        objects
        configuration
        gmock
    )
endforeach(test)
