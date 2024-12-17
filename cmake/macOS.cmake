include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)

# macOS specific configuration options

# Add a macro to indicate that we are on OSX
add_compile_options(-DOSX)

# OSX is a dev environment and we need to be able to run tests
set(enableTesting ON)

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


set(glLibs ${OPENGL_LIBRARIES} GLEW::glew glfw)