add_library(
    writers
    ObjWriter.cpp
)

target_include_directories(
    writers
    PUBLIC
    ../common
)

target_link_libraries(
    writers
    PUBLIC
    common
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()