file(GLOB_RECURSE sources "*.cpp" "arcball/*.cpp")

add_library(objects ${sources})


target_include_directories(
    objects
    PUBLIC
    ${mathIncDir}
    ${glIncludeDirectories}
    ../common
    ../mesh
    ../events
    ../config
    ../writers
    ../shaders
    .
)

target_link_libraries(
    objects
    PUBLIC
    common
    writers
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
