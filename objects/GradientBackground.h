#pragma once

#include "Renderable.h"
#include "Types.h"

namespace mv::objects {

class GradientBackground : public Renderable {

public:
    enum class GradientType {
        Linear,
        Radial
    };
    enum class GradientDirection {
        Horizontal,
        Vertical
    };
    explicit GradientBackground(GradientType const type = GradientType::Linear,
                                GradientDirection const direction = GradientDirection::Vertical);
    void setGradientDirection(GradientDirection const direction) {
        m_direction = direction;
        m_readyToRender = false;
    }

public:
    void render(const Camera &camera) override;

protected:
    void generateRenderData() override;
    void generateColors() override { /* Empty because color data is packaged into vertex buffer data*/ };

private:
    GradientType m_type;
    GradientDirection m_direction;
};

}