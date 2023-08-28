#pragma once

#include "Renderable.h"
#include "3dmath/primitives/Sphere.h"
#include "3dmath/RotationMatrix.h"
#include "ArcballVisualizationItem.h"
#include <thread>

namespace mv::objects {

    class ArcballController : public Renderable {

    public:
        ArcballController();

    public:
        void setVisualizationOn();
        void setVisualizationOff();
        void handleMouseMove(common::Point2D const& cursorPosition);

        [[nodiscard]]
        common::Point3D getCentroid() const override {
            return {0.f, 0.f, 0.f};
        }

        [[nodiscard]]
        common::Bounds getBounds() const override {
            return common::Bounds {{-1.f, -1.f, -1.f}, {+1.f, +1.f, +1.f}};
        }

        void render() override;

        void notifyWindowResized(unsigned int windowWidth, unsigned int windowHeight) override;

    private:
        void createVisualization();

    private:
        using VisualizationItem = std::unique_ptr<ArcballVisualizationItem>;
        std::vector<VisualizationItem> visualizationItems;
        math3d::Sphere sphere;
        common::Point3D pointA;
        common::Point3D pointB;
        common::Vector3D rotationAxis;
        bool visualizationOn{};
        bool active{};
        std::thread fadeOutTimer;
    };

}
