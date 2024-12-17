#include "UserInterface.h"
#include "Texture.h"
#include "OpenGLCall.h"
#include "Util.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <format>
#include <filesystem>

namespace mv::ui {

    static constexpr int MandatoryGLZeroBorder{};

    std::unordered_map<std::string, UserInterface::Button> UserInterface::buttons = {
            {"models",   {0, "Models",   "Select Models"}},
            {"colors",   {1, "Colors",   "Select Colors"}},
            {"camera",   {2, "Camera",   "Set View"}},
            {"settings", {3, "Settings", "Configure Viewer"}}
    };

    void UserInterface::Button::BoundingBox::assign(ImVec2 const& upperLeft, ImVec2 const& lowerRight) {
        upperLeftCorner.x = upperLeft.x; upperLeftCorner.y = upperLeft.y;
        lowerRightCorner.x = lowerRight.x; lowerRightCorner.y = lowerRight.y;
    }

    void UserInterface::initialize(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;

        io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 16.f);

        loadButtonTextures("assets/textures");

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifndef EMSCRIPTEN
        ImGui_ImplOpenGL3_Init("#version 410 core");
#else
        ImGui_ImplOpenGL3_Init("#version 300 es");
#endif
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = WindowCornerRadius;
        initialized = true;
    }

    void UserInterface::loadButtonTextures(std::filesystem::path const& textureDir) {
        identifyButtonTextures(textureDir);
        for (auto const& [textureName, texture] : textures) {
            GLuint textureId;
            glCallWithErrorCheck(glGenTextures, 1, &textureId)
            glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, textureId)

            glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
            glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)

            glCallWithErrorCheck(glPixelStorei, GL_UNPACK_ROW_LENGTH, 0)
            glCallWithErrorCheck(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA,
                                 static_cast<int>(texture.width),
                                 static_cast<int>(texture.height), MandatoryGLZeroBorder,
                                 GL_RGBA, GL_UNSIGNED_BYTE,
                                 texture.data.get())
            glCallWithErrorCheck(glGenerateMipmap, GL_TEXTURE_2D)
           openglTextures.emplace(textureName, textureId);
        }
        // Now that data has been loaded to the gpu, the cpu-side memory can be freed up
        textures.clear();
    }

    void UserInterface::identifyButtonTextures(std::filesystem::path const& textureDir) {
        std::filesystem::directory_iterator directoryIterator{textureDir};
        for (auto& entry : directoryIterator) {
            if (entry.is_regular_file()) {
                if (entry.path().extension() == ".png" &&
                    entry.path().stem().string().ends_with("_button_icon")) {
                    auto fileName = entry.path().stem().string();
                    textures.emplace(fileName.substr(0, fileName.find_first_of('_')), entry.path());
                }
            }
        }
    }

    void UserInterface::setPosition(Position newPosition) {
        position = newPosition;
        auto panelSize = static_cast<float>(ButtonSize * NumButtons + ((NumButtons +1) * ButtonSpacing));
        if (position == Position::Bottom) {
            origin.x = (windowSize.x - panelSize) * 0.5f;
            origin.y = windowSize.y - ButtonSize - PanelMargin - 2 * ButtonMargin;
            dimension.x = static_cast<float>(panelSize);
            dimension.y = ButtonSize + 2 * ButtonMargin;
            isVerticallyOriented = false;
        } else if (position == Position::Right) {
            origin.x = windowSize.x - ButtonSize - PanelMargin - 2 * ButtonMargin;
            origin.y = (windowSize.y - panelSize) * 0.5f;
            dimension.x = ButtonSize + 2 * ButtonMargin;
            dimension.y = static_cast<float>(panelSize);
            isVerticallyOriented = true;
        } else {
            throw std::runtime_error{std::format("Unsupported position {}", static_cast<int>(position))};
        }
    }

    bool UserInterface::wasResized(GLFWwindow* window) {
        int windowWidth {}, windowHeight{};
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        return (windowWidth != windowSize.x || windowHeight != windowSize.y); // NOLINT: narrowing conversion
    }

    void UserInterface::handleResize(GLFWwindow* window) {
        int windowWidth {}, windowHeight{};
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        windowSize.x = windowWidth; // NOLINT: narrowing conversion
        windowSize.y = windowHeight; // NOLINT: narrowing conversion
        float scaleX, scaleY;
        glfwGetWindowContentScale(window, &scaleX, & scaleY);
        ImGui::GetIO().DisplaySize = ImVec2{windowSize.x * scaleX, windowSize.y * scaleY};
        setPosition(position);
    }

    void UserInterface::beginDraw(GLFWwindow* window) {
        if (!initialized) {
            initialize(window);
        }
        if (wasResized(window)) {
            handleResize(window);
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowBgAlpha(PanelOpacity);
        ImGui::SetNextWindowPos({origin.x, origin.y}, ImGuiCond_Always);
        ImGui::SetNextWindowSize({dimension.x, dimension.y}, ImGuiCond_Always);
        drawPanel();
        ImGui::EndFrame();
    }

    void UserInterface::drawPanel() {
        ImGui::Begin("ButtonBar", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoDecoration);
        drawButtons();
        ImGui::End();
    }

    void UserInterface::setNextPosition(unsigned char buttonIndex) {
        if (buttonIndex == 0) {
            ImGui::SetCursorScreenPos({origin.x + ButtonMargin, origin.y + ButtonMargin});
        } else {
            if (isVerticallyOriented) {
                ImGui::SetCursorScreenPos(
                        {origin.x + ButtonMargin,
                         origin.y + static_cast<float>(buttonIndex * ButtonSize + (buttonIndex + 1) * ButtonSpacing)});
            } else {
                ImGui::SetCursorScreenPos(
                        {origin.x + static_cast<float>(buttonIndex * ButtonSize + (buttonIndex + 1) * ButtonSpacing),
                         origin.y + ButtonMargin});
            }
        }
    }

    void UserInterface::drawButtons() {
        calculateHoverLabelWidth();
        drawModelControls();
        drawColorControls();
        drawCameraControls();
        drawSettingsControls();
    }

    void UserInterface::calculateHoverLabelWidth() {
        auto maxWidth = 0.f;
        for (auto& [buttonId, button] : buttons) {
            auto textSize = ImGui::CalcTextSize(button.hoverLabel.c_str());
            if (textSize.x > maxWidth) {
                maxWidth = textSize.x;
            }
        }
        hoverLabelWidth = maxWidth;
    }

    void UserInterface::drawButtonLabel(std::string const& text) {
        auto textSize = ImGui::CalcTextSize(text.c_str());
        auto buttonMin = ImGui::GetItemRectMin();
        ImVec2 buttonCenter = { buttonMin.x + ButtonSize * 0.5f,
                                buttonMin.y + ButtonSize * 0.5f };
        if (position == Position::Right) {
            ImGui::SetNextWindowPos({origin.x - textSize.x - ButtonLabelSeparation, buttonCenter.y - textSize.y * 0.5f});
        } else if (position == Position::Bottom) {
            ImGui::SetNextWindowPos({buttonCenter.x - hoverLabelWidth * 0.5f, origin.y - textSize.y - ButtonLabelSeparation});
        }

        // Label
        ImGui::SetNextWindowBgAlpha(TextPanelOpacity);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, LabelCornerRadius);
        ImGui::Begin(text.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoDecoration);
        ImGui::Text("%s", text.c_str());
        ImVec2 labelOrigin = ImGui::GetWindowPos();
        ImVec2 labelSize = ImGui::GetWindowSize();
        ImGui::End();
        ImGui::PopStyleVar();

        // Triangle decoration
        ImVec2 triangleBaseCenter {labelOrigin.x + labelSize.x * 0.5f, labelOrigin.y + labelSize.y + HoverTriangleFudge};
        ImGui::SetNextWindowPos({triangleBaseCenter.x - HoverDecorationTriangleBase, triangleBaseCenter.y});
        ImGui::Begin("Marker", nullptr, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
                                        ImGuiWindowFlags_NoInputs);
        auto drawList = ImGui::GetWindowDrawList();
        ImColor color{0.0f, 0.0f, 0.0f, TextPanelOpacity * 0.5f};
        drawList->AddLine({triangleBaseCenter.x - HoverDecorationTriangleBase * 0.5f, triangleBaseCenter.y},
                          {triangleBaseCenter.x, triangleBaseCenter.y + HoverDecorationTriangleHeight}, color);
        drawList->AddLine({triangleBaseCenter.x + HoverDecorationTriangleBase * 0.5f, triangleBaseCenter.y},
                          {triangleBaseCenter.x, triangleBaseCenter.y + HoverDecorationTriangleHeight}, color);
        ImGui::End();
    }

    void UserInterface::drawModelControls() {
        setNextPosition(buttons["models"].position);
        ImGui::Image(openglTextures["models"], {ButtonSize, ButtonSize});
        if (ImGui::IsItemHovered()) {
            drawButtonLabel(buttons["models"].hoverLabel);
        }
        buttons["models"].bounds.assign(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    void UserInterface::drawColorControls() {
        setNextPosition(buttons["colors"].position);
        ImGui::Image(openglTextures["colors"], {ButtonSize, ButtonSize});
        if (ImGui::IsItemHovered()) {
            drawButtonLabel(buttons["colors"].hoverLabel);
        }
        buttons["colors"].bounds.assign(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    void UserInterface::drawCameraControls() {
        setNextPosition(buttons["camera"].position);
        ImGui::Image(openglTextures["camera"], {ButtonSize, ButtonSize});
        if (ImGui::IsItemHovered()) {
            drawButtonLabel(buttons["camera"].hoverLabel);
        }
        buttons["camera"].bounds.assign(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    void UserInterface::drawSettingsControls() {
        setNextPosition(buttons["settings"].position);
        ImGui::Image(openglTextures["settings"], {ButtonSize, ButtonSize});
        if (ImGui::IsItemHovered()) {
            drawButtonLabel(buttons["settings"].hoverLabel);
        }
        buttons["settings"].bounds.assign(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    void UserInterface::stop() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void UserInterface::endDraw() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
