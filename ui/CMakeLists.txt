set(imGuiSrc
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

file(GLOB uiSrc *.cpp)

include_directories(../common)

add_library(ui STATIC ${imGuiSrc} ${uiSrc})

target_link_libraries(ui PUBLIC events configuration)

target_include_directories(ui PUBLIC
    ${glIncludeDirectories}
    ${imgui_SOURCE_DIR}
    .
)

# In emscripten builds, assets are managed through emscripten's pre-loaded files paradigm
# See cmake/emscripten.cmake for details
if (NOT DEFINED EMSCRIPTEN)
    file(
        COPY
        fonts/Roboto-Medium.ttf
        DESTINATION
        ${CMAKE_BINARY_DIR}/assets/fonts
    )

    file(
        COPY
        textures/icons
        DESTINATION
        ${CMAKE_BINARY_DIR}/assets/textures
    )
endif()