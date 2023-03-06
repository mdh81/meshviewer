add_library(
    events 
    EventHandler.cpp
)

target_link_libraries(
    events
    glfw
)

enable_testing()
add_subdirectory(tests)
