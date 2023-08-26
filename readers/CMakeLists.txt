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
)

target_link_libraries(
    readers
    PUBLIC
    GLEW::GLEW
    glm::glm
)

if (APPLE)
    enable_testing()
    add_subdirectory(tests)
endif()
