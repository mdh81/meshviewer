add_library(
    objects 
    Camera.cpp
    CameraFactory.cpp
    Glyph.cpp
    GradientBackground.cpp)

target_include_directories(
    objects
    PUBLIC
    ${mathIncDir}
    ../common
    ../mesh
    ../events
    ../config
)

target_link_libraries(
    objects
    PUBLIC
    glm::glm
    GLEW::GLEW
)

enable_testing()
add_subdirectory(tests)
