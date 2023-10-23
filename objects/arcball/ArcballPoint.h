#pragma once
#include "ArcballVisualizationItem.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class ArcballPoint : public ArcballVisualizationItem {
    public:
        ArcballPoint(common::DisplayDimensions const& displayDimensions);
        void setPosition(common::Point3D const& position) override {
            positionCamera = position;
        }
        void render() override;

    protected:
        void setTransforms() override;
        void generateRenderData() override;
        void generateColors() override;void fadeOut() override;

    private:
        float scaleFactor;
        common::Point3D positionCamera;
        unsigned const pointSizePixels;
        unsigned textureId;
    };

}
