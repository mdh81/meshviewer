add_library(
    common 
    MeshViewerObject.cpp
)

target_include_directories(
    common 
    PUBLIC
)

if (APPLE)
    enable_testing()
    add_subdirectory(tests)
endif()
