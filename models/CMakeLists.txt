file(GLOB sources *.cpp)

add_library(
    models
    ${sources}
)

target_include_directories(
    models
    PUBLIC
    .
    ../viewer
)

target_link_libraries(
    models
    PRIVATE
    common
    readers
    mesh
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
