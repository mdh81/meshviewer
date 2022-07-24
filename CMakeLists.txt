cmake_minimum_required(VERSION 3.0.0)
project(MeshViewer VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# get gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set(
  glIncDirs
  /opt/homebrew/include
)

set (
  glLibDirs
  /opt/homebrew/lib
)

set(glLibs GLEW glfw)

add_subdirectory(events)

add_library(
    meshviewer
    Mesh.cpp
    Octree.cpp
    STLReader.cpp
    Viewer.cpp
    ShaderLoader.cpp
    Camera.cpp
    CameraFactory.cpp
    OrthographicCamera.cpp
    PerspectiveCamera.cpp
    MeshViewerObject.cpp
)

target_include_directories(
    meshviewer
    PUBLIC
    ${glIncDirs}
    events 
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

target_link_libraries(
    meshviewerapp
    PUBLIC
    meshviewer 
    ${glLibs}
    events
    "-framework OpenGL"
)

# Copy shaders to build directory
file(
    GLOB 
    shaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.shader
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/)
add_custom_target(
    shadercopy ALL
    ${CMAKE_COMMAND} -E copy ${shaders} ${CMAKE_CURRENT_BINARY_DIR}/shaders/
)

# Install shaders
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders/ DESTINATION ${INSTALL_PATH}/shaders)

enable_testing()
add_subdirectory(tests)
