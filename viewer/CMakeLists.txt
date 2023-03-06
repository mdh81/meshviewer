add_library(
    viewer 
    Viewer.cpp
)

target_include_directories(
    viewer 
    PUBLIC
    ${mathIncDir}
    ../common
    ../mesh
    ../events
    ../shaders
    ../objects
)

enable_testing()
add_subdirectory(tests)
