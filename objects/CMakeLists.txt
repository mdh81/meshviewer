file(GLOB_RECURSE sources "*.cpp" "arcball/*.cpp")

add_library(objects ${sources})


target_include_directories(
    objects
    PUBLIC
    ${mathIncDir}
    ${glIncludeDirectories}
    ../common
    ../mesh # TODO: Why is this dependency required? Potential for circular dependency
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
    shaders
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()

# Copy textures to binary directory
file(
    COPY
    textures/
    DESTINATION
    ${CMAKE_BINARY_DIR}/assets/textures
)

