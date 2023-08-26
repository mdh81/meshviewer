add_library(
    common 
    MeshViewerObject.cpp
)

target_include_directories(
    common 
    PUBLIC
)

target_link_libraries(
    common
    glm::glm
    GLEW::GLEW
)

if (APPLE)
    enable_testing()
    add_subdirectory(tests)
endif()
