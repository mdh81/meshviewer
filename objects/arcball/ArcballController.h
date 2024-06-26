#pragma once

#include "Renderable.h"
#include "3dmath/primitives/Sphere.h"
#include "3dmath/RotationMatrix.h"
#include "ArcballVisualizationItem.h"
#include "Types.h"
#include "PointerTypes.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>

namespace mv::objects {

    class ArcballController : public Renderable {

    public:
        ArcballController();

    public:
        void setVisualizationOn();
        void setVisualizationOff();

        [[nodiscard]]
        common::RotationMatrix getRotation();

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

        void handleScrollEvent(common::Point3D const& cursorPositionDevice, bool const directionChanged);

        void handleDragEvent(common::Point3D const& cursorPositionDevice);

        void reset();

    private:
        void createVisualization();
        void updateVisualization();
        math3d::types::Point3D convertCursorToCameraCoordinates(common::Point3D const& cursorPosition);
        void monitorInteraction();
        common::Point3D getCursorLocationOnArcball(common::Point3D const& cursorPositionDevice);

    private:
        using VisualizationItem = std::unique_ptr<ArcballVisualizationItem>;
        std::vector<VisualizationItem> visualizationItems;
        math3d::Sphere sphere;
        common::UniquePointer<common::Point3D> arcStartPoint;
        common::UniquePointer<common::Point3D> arcEndPoint;
        common::UniquePointer<common::Vector3D> rotationAxis;
        std::optional<std::reference_wrapper<ArcballVisualizationItem>> arcStartPointVisual;
        std::optional<std::reference_wrapper<ArcballVisualizationItem>> arcEndPointVisual;
        std::optional<std::reference_wrapper<ArcballVisualizationItem>> arcStartVectorVisual;
        std::optional<std::reference_wrapper<ArcballVisualizationItem>> arcEndVectorVisual;
        std::optional<std::reference_wrapper<ArcballVisualizationItem>> arcAxisVectorVisual;
        float theta {0.f};
        bool visualizationOn{};
        common::ProjectionMatrixPointer projectionMatrix;
        math3d::Matrix<float, 4, 4> inverseProjectionMatrix;
        common::DisplayDimensions displayDimensions;
        std::optional<common::Point2D> previousCursorPosition;
        std::optional<common::Point2D> currentCursorPosition;
        enum class Mode {
            Inactive,
            Scroll,
            Drag,
        };
        std::atomic<Mode> mode;
        bool positiveRotation {true};
        math3d::RotationMatrix<float> rotationMatrix;
        math3d::RotationMatrix<float> previousRotationMatrix{};
    };

    using ArcballControllerPointer = common::UniquePointer<ArcballController>;

}
