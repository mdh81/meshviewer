#pragma once

#include "ArcballVisualizationItem.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class ArcballSphere : public ArcballVisualizationItem {
    public:
        ArcballSphere(common::DisplayDimensions const& displayDimensions);
        void render() override;

    protected:
        void generateRenderData() override;
        void fadeOut() override;
        void generateColors() override;

    private:
        void updateColor();

    private:
        unsigned const resolution;
        common::RGBColor const color;
    };

}
