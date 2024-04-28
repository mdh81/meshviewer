#pragma once

#include "ArcballVisualizationItem.h"

namespace mv::objects {


    class ArcballDirectionVector : public ArcballVisualizationItem {

    public:
        ArcballDirectionVector(common::DisplayDimensions const& displayDimensions);
        void setPosition(common::Point3D const& position) override {
            pointOnSphere = position;
        };
        void render() override;

    protected:
        void setTransforms() override;
        void generateRenderData() override;
        void generateColors() override;
        void fadeOut() override;

    private:
        void constructGeometry(common::TwoDPoints& vertices, math3d::types::Tris& indices, float lineWidthCamera);
        void createTexture(bool isQuadTexture);

    private:
        unsigned const lineWidthPixels;
        unsigned quadTextureId;
        unsigned triTextureId;
        common::Point3D pointOnSphere;
        common::Point4D vectorColor;

    };

}
