# This function helps cmake script in each test directory to get hold of test names
# so it can configure individual tests
function(addTestSources)
    file(GLOB allTestSources CONFIGURE_DEPENDS "*.cpp")
    set(allTests)

    foreach(testSource ${allTestSources})
        get_filename_component(testName ${testSource} NAME_WE)
        list(APPEND allTests ${testName})
    endforeach()

    # Set a parent scope variable to make it accessible in the calling CMakeLists.txt
    set(allTests ${allTests} PARENT_SCOPE)
endfunction()

