#pragma once

#include "Renderable.h"
#include "3dmath/primitives/Sphere.h"
#include "3dmath/RotationMatrix.h"
#include "ArcballVisualizationItem.h"
#include "Types.h"
#include <thread>

namespace mv::objects {

    class ArcballController : public Renderable {

    public:
        ArcballController();

    public:
        void setVisualizationOn();
        void setVisualizationOff();

        [[nodiscard]]
        common::RotationMatrix getRotation(common::Point3D const& cursorPositionDevice);

        [[nodiscard]]
        common::Point3D getCentroid() const override {
            return {0.f, 0.f, 0.f};
        }

        [[nodiscard]]
        common::Bounds getBounds() const override {
            return common::Bounds {{-1.f, -1.f, -1.f}, {+1.f, +1.f, +1.f}};
        }

        void render() override;

        void notifyDisplayResized(common::DisplayDimensions const& displaySize) override;

    private:
        void createVisualization();
        void recordCursorPosition(common::Point3D const& cursorPositionDevice);
        math3d::types::Point3D convertCursorToCameraCoordinates(common::Point3D const& cursorPosition);

    private:
        using VisualizationItem = std::unique_ptr<ArcballVisualizationItem>;
        std::vector<VisualizationItem> visualizationItems;
        math3d::Sphere sphere;
        std::unique_ptr<common::Point3D> pointA;
        std::unique_ptr<common::Point3D> pointB;
        std::unique_ptr<common::Vector3D> rotationAxis;
        float theta {0.f};
        bool visualizationOn{};
        std::thread fadeOutTimer;
        common::ProjectionMatrixPointer projectionMatrix;
        math3d::Matrix<float, 4, 4> inverseProjectionMatrix;
    };

    using ArcballControllerPointer = std::unique_ptr<ArcballController>;

}
