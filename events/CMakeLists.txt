add_library(
    events 
    EventHandler.cpp
    CallbackManager.cpp
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
    enable_testing()
    add_subdirectory(tests)
endif()
