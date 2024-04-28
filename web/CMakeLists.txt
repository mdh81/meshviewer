# Copy shell file to the build directory
file (
    MAKE_DIRECTORY
    ${CMAKE_BINARY_DIR}/web
)

file (
    COPY
    shell.html
    DESTINATION
    ${CMAKE_BINARY_DIR}/web)

# Specify shell file as a link option for emscripten
set_target_properties (
    ${appName}
    PROPERTIES
    LINK_FLAGS
    "--shell-file web/shell.html"
)

# Copy javascript files to the build directory
file (
    MAKE_DIRECTORY
    ${CMAKE_BINARY_DIR}/js
)

file (
    COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/js
    DESTINATION
    ${CMAKE_BINARY_DIR}
)

# Convert shaders to html so they can be accessed by emscripten runtime using DOM
file (
    MAKE_DIRECTORY
    ${CMAKE_BINARY_DIR}/shaders
)

file (
    GLOB
    shaderSources
    ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/*.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/*.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/*.frag
)

find_program(
    pythonFound
    python3
)

if (NOT pythonFound)
    message(FATAL_ERROR "Missing Dependency: Python3 is required to setup emscripten build")
endif()

message(STATUS "Checking for regressions in shader to html conversion logic")

execute_process(
    COMMAND env PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/py/
    python3
    ${CMAKE_CURRENT_SOURCE_DIR}/py/shaderToHtmlTest.py
    RESULT_VARIABLE testExitCode
)

if (testExitCode)
    message(FATAL_ERROR "Shader to html conversion unit test failed")
endif()

file (
    REMOVE
    ${CMAKE_BINARY_DIR}/shaders/shaders.html
)

foreach (shaderSource ${shaderSources})
    execute_process (
        COMMAND
        python3
        ${CMAKE_CURRENT_SOURCE_DIR}/py/shaderToHtml.py ${shaderSource} ${CMAKE_BINARY_DIR}/shaders/shaders.html
        RESULT_VARIABLE pythonExitCode
        ERROR_VARIABLE pythonErrorString
    )
    if (pythonExitCode)
        message(FATAL_ERROR "Failed to convert shader ${shaderSource} to html. Error: ${pythonErrorString}")
    endif()
endforeach()

message(STATUS "Shaders for emscripten runtime were created successfully")