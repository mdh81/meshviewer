add_library(
    viewer 
    Viewer.cpp
    Scene.cpp
    Viewport.cpp)


target_include_directories(
    viewer 
    PUBLIC
    ${mathIncDir}
    ../common
    ../mesh
    ../events
    ../shaders
    ../objects
    ../config
)

target_link_libraries(
    viewer
    glm::glm
    GLEW::GLEW
)

enable_testing()
add_subdirectory(tests)
