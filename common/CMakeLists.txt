add_library(
    common
    external/stb_image.cpp
    external/stb_image_write.cpp
    MeshViewerObject.cpp
    Environment.cpp
)

# TODO: This has to be reexamined
target_include_directories(
    common 
    INTERFACE
    ${glIncludeDirectories}
)

target_include_directories(
    common
    PUBLIC
    .
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
