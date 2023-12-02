cmake_minimum_required(VERSION 3.24.2)

# Use c++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler has to be set before project() call
if (DEFINED ENV{DEV_CONTAINER})
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    set(CMAKE_C_COMPILER /usr/bin/clang)
endif()

project(MeshViewer VERSION 1.0.0)

if (APPLE)
    add_compile_options(-DOSX)
    set(enableTesting ON)
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
    set(enableTesting OFF)
elseif(NOT EMSCRIPTEN)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glm REQUIRED)
    # set include directories explicitly for desktop builds this avoids reliance on
    # subprojects having to use target_link_libraries command to set include paths
    # NOTE: Each module's cmake config file uses its own convention for variable names
    #       This project uses camel-case for variable names across all languages
    get_target_property(glfwIncludeDirectory glfw INTERFACE_INCLUDE_DIRECTORIES)
    # TODO: This property is pointing to the wrong path on macOS. Compilation is successful
    # only because glm headers and glfw headers above share the same root directory
    get_target_property(glmIncludeDirectory glm::glm INTERFACE_INCLUDE_DIRECTORIES)
    set(glIncludeDirectories
        ${GLEW_INCLUDE_DIRECTORY}
        ${glfwIncludeDirectory}
        ${OPENGL_INCLUDE_DIR}
        ${glmIncludeDirectory})
endif()

if (EMSCRIPTEN)
    # set preprocessor flag
    add_compile_options(-DEMSCRIPTEN)
    # compile and link options
    add_compile_options(
        --no-heap-copy
        -fexceptions
    )
    add_link_options(
        -fexceptions
        -sMIN_WEBGL_VERSION=2
        -sMAX_WEBGL_VERSION=2
        -sUSE_GLFW=3
        -sFULL_ES2
        "SHELL:-sWASM=1 -sALLOW_MEMORY_GROWTH=1 -sALLOW_TABLE_GROWTH=1 -sSTACK_SIZE=5MB"
    )
    # TODO: Remove this when glm is replaced with 3dmath
    list(APPEND glIncludeDirectories /opt/homebrew/include)
    # Disable building tests, they are not relevant for web target
    set(enableTesting OFF)
    # Build html output
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# get gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

# Set default build type to debug
set(CMAKE_BUILD_TYPE Debug)

# List of opengl libraries
if (NOT EMSCRIPTEN)
    set(glLibs ${OPENGL_LIBRARIES} GLEW::GLEW glfw)
endif()

# Sub-modules
add_subdirectory(3dmath)
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

if (EMSCRIPTEN)
    set_target_properties(
        meshviewerapp
        PROPERTIES
        LINK_FLAGS
        "--shell-file web/shell.html"
    )
endif()

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
