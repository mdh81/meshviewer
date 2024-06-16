file(GLOB sources *.cpp)

add_library(
    mesh 
    ${sources}
)

target_include_directories(
    mesh 
    PUBLIC
    ${mathIncDir}
    .
)

target_link_libraries(
    mesh
    PUBLIC
    objects
    configuration
    common
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
