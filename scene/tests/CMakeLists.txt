add_executable(
    scene_test
    scene_test.cpp
)

add_executable(
    viewport_test
    viewport_test.cpp
)

set(allTests scene_test viewport_test)

foreach(test ${allTests})
    target_include_directories(
        ${test}
        PUBLIC
        ../
        ../../common/testsupport
    )
endforeach(test)

foreach(test ${allTests})
    target_link_libraries(
        ${test}
        PUBLIC
        gtest_main
        scene
        common
        gmock
    )
endforeach(test)
