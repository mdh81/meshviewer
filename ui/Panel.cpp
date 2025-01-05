#include "Panel.h"

#include "imgui.h"
#include "3dmath/Utilities.h"
#include <thread>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <utility>

namespace mv::ui {

    namespace {
        auto constexpr windowRounding {5.f};
        auto constexpr grey {0.5f};
    }

    Panel::Panel(std::string title)
    : title{std::move(title)}
    , active{false}
    , displayFlags {ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoCollapse} {

    }

    void Panel::beginDraw(common::Point2D const& position) {
        if (!fadeInDisplay) {
            fadeInDisplay = std::make_unique<AnimatedDisplay>(DisplayType::FadeIn);
            fadeInDisplay->start();
        }
        auto const opacity = fadeInDisplay->getOpacity();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive,ImVec4(grey, grey, grey, opacity));
        ImGui::PushStyleColor(ImGuiCol_TitleBg,ImVec4(grey, grey, grey, opacity));
        ImGui::PushStyleColor(ImGuiCol_Text,
            ImVec4(2*grey, 2*grey, 2*grey, fadeInDisplay->animating ? opacity : 1.f));
        ImGui::SetNextWindowBgAlpha(opacity);
        ImGui::SetNextWindowPos({position.x, position.y});
        ImGui::SetNextWindowSize({ImGui::CalcTextSize(title.c_str()).x + ImGui::GetStyle().WindowPadding.x, 0});
        ImGui::Begin(this->title.c_str(), nullptr, displayFlags);
    }

    void Panel::endDraw() {
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
        active = true;
        ImGui::End();
    }

    float Panel::getCurrentOpacity() const {
        return fadeInDisplay ? fadeInDisplay->opacity : 1.0f;
    }

    Panel::AnimatedDisplay::AnimatedDisplay(DisplayType const type, std::chrono::milliseconds const& animationDuration,
        float const minOpacity, float const maxOpacity)
    : type {type}
    , opacity {type == DisplayType::FadeIn ? minOpacity : maxOpacity}
    , minOpacity(minOpacity)
    , maxOpacity(maxOpacity)
    , opacityIncrement { (maxOpacity - minOpacity) / static_cast<float>(animationDuration.count()) }
    , animating{} {
    }

    void Panel::AnimatedDisplay::start() {
        startTime = std::chrono::high_resolution_clock::now();
        animating = true;
    }

    void Panel::AnimatedDisplay::stop() {
        animating = false;
    }

    bool Panel::isActive() const {
        return active;
    }

    bool Panel::requiresRedraw() const {
        return fadeInDisplay && fadeInDisplay->animating;
    }

    float Panel::AnimatedDisplay::getOpacity() {
        if (!animating) return opacity;
        auto timeNow = std::chrono::high_resolution_clock::now();
        auto elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count());
        opacity = type == DisplayType::FadeIn ? elapsedTime * opacityIncrement : -1.f * elapsedTime * opacityIncrement;
        if (opacity < minOpacity || opacity > maxOpacity) {
            if (type == DisplayType::FadeIn) {
                opacity = std::max(opacity, maxOpacity);
            } else {
                opacity = std::min(opacity, minOpacity);
            }
            stop();
        }
        return opacity;
    }

}
