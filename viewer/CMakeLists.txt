file(GLOB sources "*.cpp")

add_library(
    viewer 
    ${sources})

target_include_directories(
    viewer 
    PUBLIC
    ${mathIncDir}
    ${glIncludeDirectories}
    ../common
    ../mesh
    ../events
    ../shaders
    ../objects
    ../config
    ../ui
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()