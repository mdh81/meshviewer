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

# Copy sample ply and stl files to binary directory
file(
    GLOB
    models
    ${CMAKE_CURRENT_SOURCE_DIR}/samples/*.stl
    ${CMAKE_CURRENT_SOURCE_DIR}/samples/*.ply
)

foreach(model ${models})
    file(
        COPY
        ${model}
        DESTINATION
        ${CMAKE_BINARY_DIR}/testfiles/
    )
endforeach()
