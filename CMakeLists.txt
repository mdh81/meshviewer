cmake_minimum_required(VERSION 3.24.2)

# Use c++ 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(MeshViewer VERSION 1.0.0)

# Set default build type to debug
if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified. Setting build type to debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()
string(TOLOWER "${CMAKE_BUILD_TYPE}" buildType)

# Load platform specific configuration
# NOTE: Order is important to make sure emscripten build is configured as such
# on all platforms
if (EMSCRIPTEN)
    message(STATUS "Configuring Emscripten build")
    include(cmake/emscripten.cmake)
elseif (APPLE)
    message(STATUS "Configuring macOS build")
    include(cmake/macOS.cmake)
elseif (LINUX)
    message(STATUS "Configuring Linux build")
    include(cmake/ubuntu.cmake)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if (buildType STREQUAL debug)
    add_compile_options(-DDEBUG)
    # Enable address sanitizer if requested
    if (USE_ADDRESS_SANITIZER)
        message(STATUS "Turning on address sanitizer")
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
endif()

# Define app name as a variable so submodules can set target properties for the app
set(appName meshViewer)

add_executable(
    ${appName}
    main.cpp
)

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
add_subdirectory(ui)
add_subdirectory(viewer)
add_subdirectory(config)
add_subdirectory(writers)
add_subdirectory(models)



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
    viewer
    events
    models
    ui
    ${glLibs}
)

if (EMSCRIPTEN)
    #TODO: Resolve the order dependency issue between web and shaders submodules
    # Once done, move this add_subdirectory call to emscripten.cmake
    add_subdirectory(web)
endif()
