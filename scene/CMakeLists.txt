add_library(
    scene 
    Scene.cpp
)

target_include_directories(
    scene 
    PUBLIC
    ../common
    ${mathIncDir}
)

enable_testing()
add_subdirectory(tests)
