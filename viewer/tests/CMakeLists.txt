add_executable(
    viewer_test
    viewer_test.cpp
)

set(allTests viewer_test)

foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ../../common
        ../../mesh
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
        config
    )
endforeach(test)
