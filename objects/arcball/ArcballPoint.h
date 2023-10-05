#pragma once
#include "ArcballVisualizationItem.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class ArcballPoint : public ArcballVisualizationItem {
    public:
        ArcballPoint();
        void setModelTransform(math3d::Matrix<float, 4, 4> const& transform) override;
        void render() override;

    protected:
        void setTransforms() override;
        void generateRenderData() override;
        void generateColors() override;void fadeOut() override;

    private:
        float calculatePointSize();

    private:
        math3d::Matrix<float, 4, 4> pointTransform;
        float const pointSize;
        unsigned textureId;
    };

}
