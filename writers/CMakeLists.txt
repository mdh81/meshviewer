add_library(
    writers
    ObjWriter.cpp
)

target_include_directories(
    writers
    PUBLIC
    ../common
)

if (APPLE)
    enable_testing()
    add_subdirectory(tests)
endif()