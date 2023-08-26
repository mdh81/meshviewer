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

# TODO: There is some weird linker error with gtest in callback tests.
# There is no use case to run tests on linux right now
if (APPLE)
    enable_testing()
    add_subdirectory(tests)
endif()
