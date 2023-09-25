#pragma once

#include "Drawable3D.h"
#include "3dmath/OrthographicProjectionMatrix.h"
#include "Types.h"

namespace mv::objects {

    class ArcballVisualizationItem : public Drawable3D {
    public:

        ArcballVisualizationItem(std::string const& vertexShaderFileName, std::string const& fragmentShaderFileName);

        [[nodiscard]]
        common::Point3D getCentroid() const override {
            return {};
        }

        [[nodiscard]]
        common::Bounds getBounds() const override {
            return {};
        }

        void render() override;

        void setProjectionMatrix(common::ProjectionMatrixPointer const& projectionMatrix) {
            this->projectionMatrix = projectionMatrix;
        }

    protected:
        void setTransforms() override;
        virtual void fadeOut() = 0;

    protected:
        common::ProjectionMatrixPointer projectionMatrix;
        unsigned numConnectivityEntries{};
        float opacity;
    };

}