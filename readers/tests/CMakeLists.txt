include(GoogleTest)
file(GLOB sources "*.cpp")

foreach(source ${sources})
    get_filename_component(testExecutable ${source} NAME_WE)
    add_executable(${testExecutable} ${source})
    target_include_directories(
        ${testExecutable}
        PUBLIC
        ../
        ../../mesh
    )
    # TODO: Create mocks for all dependencies. Reader tests should link only against reader library
    target_link_libraries(
        ${testExecutable}
        PUBLIC
        gtest_main
        readers
        mesh
        common
        shaders
        objects
        events
        ${glLibs}
    )
    gtest_discover_tests(${testExecutable} PROPERTIES ENVIRONMENT "modelsDir=${CMAKE_BINARY_DIR}/testfiles")
endforeach(source)
