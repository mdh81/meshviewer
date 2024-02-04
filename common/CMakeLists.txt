add_library(
    common 
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
