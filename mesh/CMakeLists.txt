add_library(
    mesh 
    Vertex.cpp
    Face.cpp
    Mesh.cpp
    Octree.cpp
)

target_include_directories(
    mesh 
    PUBLIC
    ${mathIncDir}
    ../common
    ../objects
)

target_link_libraries(
    mesh
    PUBLIC
    glm::glm
    GLEW::GLEW
)

enable_testing()
add_subdirectory(tests)
