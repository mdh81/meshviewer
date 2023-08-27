cmake_minimum_required(VERSION 3.0.0)

# Use c++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler has to be set before project() call
if (DEFINED ENV{DEV_CONTAINER})
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    set(CMAKE_C_COMPILER /usr/bin/clang)
endif()

project(MeshViewer VERSION 0.1.0)


if (APPLE)
    add_compile_options(-DOSX)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-DDEBUG)
endif()

# The linux dev environment container uses debian, which doesn't install dependencies with 
# cmake configuration files. Set up the dependencies manually as imported/interface libraries
if (DEFINED ENV{DEV_CONTAINER})
    set(LIBRARY_PATH /usr/lib/aarch64-linux-gnu/)
    add_library(GLEW::glew SHARED IMPORTED GLOBAL)
    set_target_properties(GLEW::glew PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH}/libGLEW.so)
    target_include_directories(GLEW::glew INTERFACE /usr/include)
    add_library(glfw SHARED IMPORTED)
    set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH}/libglfw.so)
    target_include_directories(glfw INTERFACE /usr/include)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE /usr/include)
    add_library(glm::glm ALIAS glm)
    add_library(GLEW::GLEW ALIAS GLEW::glew)
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBRARIES OpenGL::GL)
else()
    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glm REQUIRED)
endif()

# get gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

# get 3dmath
FetchContent_Declare(
    3dmath
    GIT_REPOSITORY https://github.com/mdh81/3dmath.git
)
FetchContent_MakeAvailable(3dmath)
include_directories(${3dmath_SOURCE_DIR}/include)

# Set default build type to debug
set(CMAKE_BUILD_TYPE Debug)

# List of opengl libraries
set(glLibs ${OPENGL_LIBRARIES} GLEW::GLEW glfw)

# Sub-modules
add_subdirectory(events)
add_subdirectory(mesh)
add_subdirectory(objects)
add_subdirectory(common)
add_subdirectory(readers)
add_subdirectory(shaders)
add_subdirectory(viewer)
add_subdirectory(config)
add_subdirectory(writers)

add_executable(
    meshviewerapp
    main.cpp)

target_include_directories(
    meshviewerapp
    PUBLIC
    common
    events
    mesh
    readers
    shaders
    viewer
    config
)

target_link_libraries(
    meshviewerapp
    PUBLIC
    common
    events
    readers
    mesh
    viewer
    events
    objects
    shaders
    configuration
    writers
    ${glLibs}
)

# Copy test files to binary directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testfiles/)
file(
    GLOB
    models
    ${CMAKE_CURRENT_SOURCE_DIR}/testfiles/*.stl
    ${CMAKE_CURRENT_SOURCE_DIR}/testfiles/*.ply
)

add_custom_target(
    testFiles ALL
    ${CMAKE_COMMAND} -E copy ${models} ${CMAKE_BINARY_DIR}/testfiles/
)
