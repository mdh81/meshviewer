add_library(
    readers 
    STLReader.cpp
)

target_include_directories(
    readers 
    PUBLIC
    ../mesh 
    ../common
    ${glIncDirs}
    ${mathIncDir}
)

enable_testing()
add_subdirectory(tests)
