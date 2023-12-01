add_library(
    events 
    EventHandler.cpp
)

target_include_directories(
    events
    PUBLIC
    ../common
)

target_link_libraries(
    events
    glfw
    common
)

if (enableTesting)
    message(STATUS "Enabling testing for events")
    enable_testing()
    add_subdirectory(tests)
endif()
