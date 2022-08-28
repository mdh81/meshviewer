add_library(
    viewer 
    Camera.cpp
    CameraFactory.cpp
    Viewer.cpp
)

target_include_directories(
    viewer 
    PUBLIC
    ${glIncDirs}
    ../common
    ../mesh
    ../events
    ../shaders
)

enable_testing()
add_subdirectory(tests)
