add_executable(
    viewer_test
    viewer_test.cpp
)

add_executable(
    camera_factory_test
    camera_factory_test.cpp
)

set(allTests viewer_test camera_factory_test)

foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ${glIncDirs}
        ../
        ../../common
        ../../mesh
    )
endforeach(test)

foreach(test ${allTests})
    target_link_directories(
        ${test}
        PUBLIC
        ${glLibDirs}
    )
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
    )
endforeach(test)
