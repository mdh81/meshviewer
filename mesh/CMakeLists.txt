add_library(
    mesh 
    Mesh.cpp
    Octree.cpp
)

target_include_directories(
    mesh 
    PUBLIC
    ${glIncDirs}
    ../common
)

enable_testing()
add_subdirectory(tests)
