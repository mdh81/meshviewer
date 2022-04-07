cmake_minimum_required(VERSION 3.0.0)
project(MeshViewer VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# get gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
FetchContent_MakeAvailable(googletest)

add_library(
    meshviewer
    Mesh.cpp
    Octree.cpp
    STLReader.cpp
    Viewer.cpp
    ShaderLoader.cpp
)

set(
  glIncDirs
  /usr/local/include
)

set (
  glLibDirs
  /usr/local/lib
)

target_include_directories(
    meshviewer
    PUBLIC
    ${glIncDirs}
)

target_link_directories(
    meshviewer
    PUBLIC
    ${glLibDirs}
)

add_executable(
    meshviewerapp
    main.cpp
)

set(glLibs GLEW glfw)
target_link_libraries(
    meshviewerapp
    PUBLIC
    meshviewer 
    ${glLibs}
)

target_link_libraries(
    meshviewerapp
    PUBLIC
    "-framework OpenGL"
)

# Copy shaders to build directory
file(
    GLOB 
    shaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.shader
)

add_custom_target(
    shadercopy ALL
    ${CMAKE_COMMAND} -E copy ${shaders} ${CMAKE_CURRENT_BINARY_DIR}/shaders/
)

# Install shaders
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders/ DESTINATION ${INSTALL_PATH}/shaders)

# This is the tester app
add_executable(
    gl101
    gl101.cpp
)
target_link_libraries(
    gl101 
    PUBLIC
    meshviewer
    ${glLibs}
)
target_link_libraries(
    gl101 
    PUBLIC
    "-framework OpenGL"
)
target_include_directories(
    gl101 
    PUBLIC
    ${glIncDirs}
)

target_link_directories(
    gl101 
    PUBLIC
    ${glLibDirs}
)

enable_testing()
add_subdirectory(tests)
