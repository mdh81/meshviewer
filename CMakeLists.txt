cmake_minimum_required(VERSION 3.0.0)
project(MeshViewer VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)

# get gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_BUILD_TYPE Debug)

# TODO: Make this downloadable from github or create a cmake config for it like other packages
set (
  mathIncDir
  /usr/local/include
)

set(glLibs GLEW::GLEW glfw)

add_subdirectory(events)
add_subdirectory(mesh)
add_subdirectory(objects)
add_subdirectory(common)
add_subdirectory(readers)
add_subdirectory(shaders)
add_subdirectory(viewer)
add_subdirectory(config)

add_executable(
    meshviewerapp
    main.cpp
)

target_include_directories(
    meshviewerapp
    PUBLIC
    common
    events
    mesh
    readers
    shaders
    viewer
)

target_link_libraries(
    meshviewerapp
    PUBLIC
    ${glLibs}
    GLEW::GLEW
    glfw
    common
    events
    mesh
    readers
    shaders
    events
    viewer
    objects
    "-framework OpenGL"
)

# Copy test files to binary directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testfiles/)
file(
    GLOB
    models
    ${CMAKE_CURRENT_SOURCE_DIR}/testfiles/*.stl
)
add_custom_target(
    testFiles ALL
    ${CMAKE_COMMAND} -E copy ${models} ${CMAKE_BINARY_DIR}/testfiles/
)

enable_testing()
