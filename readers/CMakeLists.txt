file(GLOB sources "*.cpp")

add_library(
    readers
    ${sources}
)

target_include_directories(
    readers 
    PUBLIC
    ../mesh 
    ../common
    ../objects
    ${mathIncDir}
    ${glIncludeDirectories}
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
