#pragma once

#include "Drawable3D.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class ArcballVisualizationItem : public Drawable3D {
    public:

        ArcballVisualizationItem(common::DisplayDimensions const& displayDimensions, std::string const& vertexShaderFileName, std::string const& fragmentShaderFileName);

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

        // For visualization items like points that need a model transform
        virtual void setPosition(common::Point3D const& position) {}

        // Notification receiver for window resize event
        void notifyDisplayResized(common::DisplayDimensions const& displayDimensions) override {
            Renderable::notifyDisplayResized(displayDimensions);
            this->displayDimensions = displayDimensions;
        };

        // Convert from screen to model units
        float getScaleFactorInCameraSpace(unsigned short sizeInPixels) const;

    protected:
        void setTransforms() override;
        virtual void fadeOut() = 0;

    protected:
        common::ProjectionMatrixPointer projectionMatrix;
        unsigned numConnectivityEntries{};
        float opacity;
        common::DisplayDimensions displayDimensions;
    };

}