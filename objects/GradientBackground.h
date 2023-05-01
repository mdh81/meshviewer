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
    void setGradientType(GradientType const type) {
        m_type = type;
        m_readyToRender = false;
    }

public:
    void render() override;

    [[nodiscard]] common::Point3D getCentroid() const override {
        return {0, 0, 0};
    }

    [[nodiscard]] common::Bounds getBounds() const override {
        return {{-1, 1}, {-1, -1}, {0, 0}};
    }

protected:
    void generateRenderData() override;
    void generateColors() override { /* Empty because color data is packaged into vertex buffer data*/ };

private:
    void generateLinearGradient();
    void generateSphericalGradient();
    void toggleGradientType() {
        setGradientType(m_type == GradientType::Linear ? GradientType::Radial : GradientType::Linear);
    }

private:
    GradientType m_type;
    GradientDirection m_direction;
    const common::byte m_numberOfStops;
    unsigned short m_numberOfConnectivityEntries;
};

}