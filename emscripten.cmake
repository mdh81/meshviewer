# set experimental flag on, without this std::format doesn't compile
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")

# set preprocessor flag
add_compile_options(-DEMSCRIPTEN)

# compile and link options
add_compile_options(
    --no-heap-copy
    -fexceptions
    -pthread
)

# Preloaded assets
set(preloadedFiles "")
# Sample files
file(GLOB sampleFiles ${CMAKE_CURRENT_SOURCE_DIR}/testfiles/*)
foreach(sampleFile ${sampleFiles})
    get_filename_component(destinationFile ${sampleFile} NAME)
    set(preloadedFiles "${preloadedFiles} --preload-file ${sampleFile}@testfiles/${destinationFile}")
endforeach(sampleFile)
# Textures
file(GLOB textureFiles ${CMAKE_CURRENT_SOURCE_DIR}/objects/textures/*)
foreach(textureFile ${textureFiles})
    get_filename_component(destinationFile ${textureFile} NAME)
    set(preloadedFiles "${preloadedFiles} --preload-file ${textureFile}@textures/${destinationFile}")
endforeach(textureFile)

# Link options
# NOTE: It is vital to use SHELL: prefix to ensure that preloaded file paths that are composed above
# are passed as-is without cmake attempting to help by inserting opening and closing quotes
add_link_options(
    "SHELL: --use_preload_cache ${preloadedFiles}"
    --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/config/defaults.cfg@config/defaults.cfg
    --emrun
    -fexceptions
    -pthread
    -sUSE_WEBGL2=1
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    -sUSE_GLFW=3
    -sFULL_ES2
    -sEXPORTED_RUNTIME_METHODS=allocate
    -sPTHREAD_POOL_SIZE=2
    "SHELL:-sWASM=1 -sUSE_WEBGL2=1 -sALLOW_MEMORY_GROWTH=1 -sALLOW_TABLE_GROWTH=1 -sSTACK_SIZE=5MB"
)

# TODO: Remove this when glm is replaced with 3dmath
list(APPEND glIncludeDirectories /usr/local/include)

# Disable building tests, they are not relevant for web target
set(enableTesting OFF)
# Build html output
set(CMAKE_EXECUTABLE_SUFFIX ".html")

# Enable debug if requested
if (DEBUG)
    add_compile_options(-g3)
    add_link_options(
        -sASSERTIONS=1
        -g3
    )
endif()

# The web sub-directory contains emscripten specific sources
#add_subdirectory(web)