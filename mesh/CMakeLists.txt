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
    objects
    configuration
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
