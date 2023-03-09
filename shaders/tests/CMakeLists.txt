add_executable(
    shaderloader_test 
    shaderloader_test.cpp
)

# Convenience variables
set(allTests shaderloader_test)

# All tests include headers from the source directory 
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
    )
endforeach(test)

# All tests link against shaders, common, GL, mesh, events, and gtest
foreach(test ${allTests})
    target_link_libraries(
        ${test} 
        PUBLIC
        gtest_main
        shaders
        mesh 
        events
        common
        ${glLibs}
        "-framework OpenGL"
    )
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test})
endforeach(test)
