#include "ArcballController.h"
#include "ArcballSphere.h"
#include "ArcballPoint.h"
#include <thread>

namespace mv::objects {

    ArcballController::ArcballController()
    : Renderable()
    , sphere({0.f, 0.f, 0.f}, 1.f) {

    }

    void ArcballController::handleMouseMove(const common::Point2D &cursorPosition) {

        if (!active) {
            // Compute pointA on the sphere
        } else {
            // Compute pointB on the sphere
        }

    }

    void ArcballController::setVisualizationOn() {
        visualizationOn = true;
    }

    void ArcballController::setVisualizationOff() {
        //fadeOutTimer.join();
        visualizationOn = false;
    }

    void ArcballController::createVisualization() {
        visualizationItems.clear();
        visualizationItems.emplace_back(std::make_unique<ArcballSphere>());
        //visualizationItems.emplace_back(std::make_unique<ArcballPoint>());
        //visualizationItems.emplace_back(std::make_unique<ArcballPoint>());
        for (auto& visualizationItem : visualizationItems) {
            visualizationItem->setInitialProjection(aspectRatio);
        }
    }

    void ArcballController::render() {
        if (visualizationOn) {
            if (visualizationItems.empty()) {
                createVisualization();
            }
            for(auto& visualizationItem : visualizationItems) {
                visualizationItem->render();
            }
        }
    }

    void ArcballController::notifyWindowResized(unsigned int windowWidth, unsigned int windowHeight) {
        Renderable::notifyWindowResized(windowWidth, windowHeight);
        for(auto& visualizationItem : visualizationItems) {
            visualizationItem->notifyWindowResized(windowWidth, windowHeight);
        }
    }

}
