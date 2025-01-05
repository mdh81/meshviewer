
#include "ModelList.h"
#include "imgui.h"

namespace mv::ui {

    ModelList::ModelList(std::string title)
    : Panel(std::move(title)) {
    }

    void ModelList::draw(common::Point2D const& position) {
        beginDraw(position);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.980f, 0.451f, 0.260f, 0.310f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.260f, 0.451f, 0.980f, 0.8f));
        const char* modelList[] = {"Model 1", "Model 2", "Model 3"};
        int current = 0;
        ImGui::ListBox("##", &current, modelList , 3);
        ImGui::PopStyleColor(3);
        endDraw();
    }
}
