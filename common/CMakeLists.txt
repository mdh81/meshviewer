add_library(
    common 
    MeshViewerObject.cpp
    Renderable.cpp
)

target_include_directories(
    common 
    PUBLIC
    ../shaders
    ../objects
)

target_link_libraries(
    common
    shaders
)

enable_testing()
add_subdirectory(tests)
