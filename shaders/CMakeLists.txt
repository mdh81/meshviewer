file (
    GLOB
    shaderSources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(
    shaders 
    ${shaderSources}
)

target_include_directories(
    shaders
    PUBLIC
    ../common
    ${glIncludeDirectories}
)

# Copy shaders to build directory
file(
    GLOB 
    shaders 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.glsl
)

add_custom_target(
    shaderCopy ALL
    ${CMAKE_COMMAND} -E copy ${shaders} ${CMAKE_CURRENT_BINARY_DIR}/
)

# Install shaders
INSTALL(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/shaders/ DESTINATION ${INSTALL_PATH}/shaders)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
