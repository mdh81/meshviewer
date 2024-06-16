file(GLOB sources "*.cpp")

add_library(
    readers
    ${sources}
)

target_include_directories(
    readers 
    PUBLIC
    .
    ../mesh/
    ../objects/
    ${mathIncDir}
)

target_link_libraries(
    readers
    PRIVATE
    common
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
