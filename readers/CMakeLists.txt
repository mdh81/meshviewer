add_library(
    readers 
    STLReader.cpp
)

target_include_directories(
    readers 
    PUBLIC
    ../mesh 
    ../common
    ../objects
    ${mathIncDir}
)

target_link_libraries(
    readers
    PUBLIC
    GLEW::GLEW
    glm::glm
)

enable_testing()
add_subdirectory(tests)
