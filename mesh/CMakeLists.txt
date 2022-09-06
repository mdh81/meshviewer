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
    ${glIncDirs}
    ${mathIncDir}
    ../common
)

enable_testing()
add_subdirectory(tests)
