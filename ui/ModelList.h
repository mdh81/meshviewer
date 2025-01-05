#pragma once

#include "Panel.h"

namespace mv::ui {

    class ModelList : public Panel {
    public:
        explicit ModelList(std::string title);
        void draw(common::Point2D const& position) override;

    };

}
