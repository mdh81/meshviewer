add_executable(
    callback_factory_test 
    callback_factory_test.cpp)

add_executable(
    event_handler_test 
    event_handler_test.cpp
)

# Convenience variables
set(allTests callback_factory_test event_handler_test)

# All tests include headers from the source directory 
foreach(test ${allTests})
    target_include_directories(
        ${test} 
        PUBLIC
        ../
        ${glIncDirs}
    )
endforeach(test)

# All tests link against gtest, events, and gl libraries 
foreach(test ${allTests})
    target_link_directories(
        ${test}
        PUBLIC
        ${glLibDirs}
    )
    target_link_libraries(
        ${test}
        PUBLIC
        ${glLibs}
        "-framework OpenGL"
        gtest_main
        events
    )  
endforeach(test)

# Enable test discovery for each test
include(GoogleTest)
foreach(test ${allTests})
    gtest_discover_tests(${test})
endforeach(test)
