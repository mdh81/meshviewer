add_library(
    common
    external/stb_image.cpp
    external/stb_image_write.cpp
    MeshViewerObject.cpp
    Environment.cpp
)

target_include_directories(
    common 
    INTERFACE
    ${glIncludeDirectories}
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
