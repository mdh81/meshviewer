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

enable_testing()
add_subdirectory(tests)
