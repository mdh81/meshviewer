add_library(
    objects
    Drawable.cpp
    Camera.cpp
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
    ../writers
    ../shaders
    .
)

target_link_libraries(
    objects
    PUBLIC
    common
    glm::glm
    GLEW::GLEW
    writers
)

if (APPLE)
    enable_testing()
    add_subdirectory(tests)
endif()
