cmake_minimum_required(VERSION 3.24.2)

# Use c++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(MeshViewer VERSION 1.0.0)
# Load platform specific configuration
# NOTE: Order is important to make sure emscripten build is configured as such
# on all platforms
if (EMSCRIPTEN)
    message(STATUS "Configuring Emscripten build")
    include(emscripten.cmake)
elseif (APPLE)
    message(STATUS "Configuring macOS build")
    include(macOS.cmake)
elseif (LINUX)
    message(STATUS "Configuring Linux build")
    include(ubuntu.cmake)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Set default build type to debug
if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified. Setting build type to debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-DDEBUG)
endif()

# get gtest
if (enableTesting)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
    include(common/testsupport/TestSupport.cmake)
endif()


# Define app name as a variable so submodules can set target properties for the app
set(appName meshViewer)

# Submodules
add_subdirectory(3dmath)
# Every other submodule is dependent on 3dmath
include_directories(3dmath/include)
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
    ${appName}
    main.cpp)

target_include_directories(
    ${appName}
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
    ${appName}
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
foreach(model ${models})
file(
    COPY
    ${model}
    DESTINATION
    ${CMAKE_BINARY_DIR}/testfiles/
)
endforeach()

# Copy textures to the binary directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/textures/)

file(
    GLOB
    textures
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/textures/*.png
    # Add new directories containing textures here
)

add_custom_target(
    textureCopy
    ${CMAKE_COMMAND} -E copy ${textures} ${CMAKE_BINARY_DIR}/textures/
)

add_dependencies(${appName} textureCopy)


if (EMSCRIPTEN)
    #TODO: Resolve the order dependency issue between web and shaders submodules
    # Once done, move this add_subdirectory call to emscripten.cmake
    add_subdirectory(web)
endif()