add_library(
    scene 
    Scene.cpp
    Viewport.cpp
)

target_include_directories(
    scene 
    PUBLIC
    ../common
    ${mathIncDir}
)

target_link_libraries(
    scene
    GLEW::glew
)

enable_testing()
add_subdirectory(tests)
