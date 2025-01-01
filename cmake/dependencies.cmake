include(FetchContent)

# NOTE:
# 1. enableTesting must be defined by platform-specific cmake scripts before this file is included
# 2. Cmake call enable_testing() has to be called before any add_subdirectory calls, otherwise ctest test discovery
#    won't work (https://cmake.org/pipermail/cmake/2010-February/034919.html)

# Gtest
if (enableTesting)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    FetchContent_MakeAvailable(googletest)
    include(${CMAKE_SOURCE_DIR}/common/testsupport/TestSupport.cmake)
endif()

# Imgui
# NOTE: Since Imgui doesn't include a CMakeLists.txt, it won't be
# built, which is the desired outcome. FetchContent_MakeAvailable
# will introduce imgui_SOURCE_DIR variable, which can be used to
# include the sources in the meshviewer's modules
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

# GL libraries
# TODO: Manage GLEW, GLFW3 through fetch content
if (NOT DEFINED EMSCRIPTEN AND NOT DEFINED LINUX)
    find_package(GLEW REQUIRED)
    find_package(GLFW3 REQUIRED)
    find_package(OpenGL REQUIRED)
    # TODO: Get rid of GLM
    find_package(glm REQUIRED)
endif()
