# Ubuntu linux is used for CI/CD builds. The following items are important:
#   The ubuntu build is run in containerized environment by the custom github CI workflow
#   GCC is the compiler of choice just to make sure the code is conformant with both clang and gcc
#   Mesa GL is used as the OpenGL implementation


set(LIBRARY_PATH /usr/lib/aarch64-linux-gnu/)
add_library(GLEW::glew SHARED IMPORTED GLOBAL)
set_target_properties(GLEW::glew PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH}/libGLEW.so)
target_include_directories(GLEW::glew INTERFACE /usr/include)
add_library(glfw SHARED IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH}/libglfw.so)
target_include_directories(glfw INTERFACE /usr/include)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE /usr/local/include)
add_library(glm::glm ALIAS glm)
add_library(GLEW::GLEW ALIAS GLEW::glew)
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARIES OpenGL::GL)
set(enableTesting ON)
set(glLibs ${OPENGL_LIBRARIES} GLEW::glew glfw)
