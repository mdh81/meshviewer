#pragma once
#include "ArcballVisualizationItem.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class ArcballPoint : public ArcballVisualizationItem {
    public:
        ArcballPoint();
        void render() override;

    protected:
        void fadeOut() override;

    protected:
        void generateRenderData() override;
        void generateColors() override;

    };

}
