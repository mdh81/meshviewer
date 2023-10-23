#pragma once

#include "ArcballVisualizationItem.h"

namespace mv::objects {


    class ArcballVector : public ArcballVisualizationItem {

    public:
        ArcballVector(common::DisplayDimensions const& displayDimensions);
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
        void constructGeometry(common::Points& vertices, math3d::types::Tris& indices, float lineWidthCamera);

    private:
        math3d::Matrix<float, 4, 4> pointTransform;
        unsigned const lineWidthPixels;
        unsigned textureId;
        common::Point3D pointOnSphere;

    };

}
