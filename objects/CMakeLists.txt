add_library(
    objects 
    Camera.cpp
    CameraFactory.cpp
    Glyph.cpp
)

target_include_directories(
    objects
    PUBLIC
    ${mathIncDir}
    ../common
    ../mesh
    ../events
)

target_link_libraries(
    objects
    PUBLIC
    glm::glm
    GLEW::GLEW
)

enable_testing()
add_subdirectory(tests)
