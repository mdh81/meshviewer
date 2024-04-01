#include "ArcballController.h"
#include "ArcballSphere.h"
#include "ArcballPoint.h"
#include "Util.h"
#include "3dmath/MatrixOperations.h"
#include "ArcballDirectionVector.h"
#include "CallbackFactory.h"
#include "EventHandler.h"
#include <thread>
#include <chrono>

namespace mv::objects {

    namespace {
        constexpr float oneDegreeInRadian = std::numbers::pi / 180;
    }

    ArcballController::ArcballController()
    : Renderable()
    , sphere({0.f, 0.f, 0.f}, 1.f)
    , mode {Mode::Inactive} {
    }

    void ArcballController::reset() {
        positiveRotation = true;
        mode = Mode::Inactive;
    }

    void ArcballController::handleScrollEvent(common::Point3D const& cursorPositionDevice, bool const directionChanged) {
        if (mode == Mode::Inactive) {
            arcStartPoint = std::make_unique<common::Point3D>(getCursorLocationOnArcball(cursorPositionDevice));
            arcEndPoint = nullptr;
        } else {
            if (!arcEndPoint) {
                arcEndPoint = std::make_unique<common::Point3D>(getCursorLocationOnArcball(cursorPositionDevice));
                rotationAxis = std::make_unique<common::Vector3D>(*arcStartPoint * *arcEndPoint);
                theta = asin(rotationAxis->length());
            } else {
                if (directionChanged) {
                    positiveRotation = !positiveRotation;
                }
                theta += oneDegreeInRadian * (positiveRotation ? 1.f : -1.f);
                auto planeAxisX = *arcStartPoint;
                auto planeAxisY = planeAxisX * *rotationAxis;
                *arcEndPoint = (cos(theta) * planeAxisX) +  (sin(theta) * planeAxisY);
            }
            rotationAxis->normalize();
        }
        mode = Mode::Scroll;
        updateVisualization();
    }

    common::Point3D ArcballController::getCursorLocationOnArcball(common::Point3D const& cursorPositionDevice) {
        math3d::types::Point3D cursorPositionCamera = convertCursorToCameraCoordinates(cursorPositionDevice);
        // In the camera space the ray from the cursor position is always in the negative z-direction
        common::Vector3D rayDirection = {0, 0, -1};
        common::Point3D arcballPoint;
        auto intersectionResult = sphere.intersectWithRay({cursorPositionCamera, rayDirection});
        if (intersectionResult.status == math3d::IntersectionStatus::Intersects) {
            arcballPoint = Util::asFloat(intersectionResult.intersectionPoint);
        } else {
            arcballPoint = Util::asFloat(cursorPositionCamera.normalize());
        }
        return arcballPoint;
    }

    void ArcballController::updateVisualization() {
        if (arcStartPoint && arcStartPointVisual) {
            arcStartPointVisual->get().setPosition(*arcStartPoint);
            arcStartVectorVisual->get().setPosition(*arcStartPoint);
            if (arcEndPoint && arcEndVectorVisual) {
                arcEndVectorVisual->get().setPosition(*arcEndPoint);
                arcEndPointVisual->get().setPosition(*arcEndPoint);
                arcAxisVectorVisual->get().setPosition(*rotationAxis);
            }
        }
    }

    common::RotationMatrix ArcballController::getRotation() {

        // If the arcball was just reset then return the previous rotation matrix
        // NOTE: rotationMatrix is initialized to identity by its constructor, so we will return the right matrix
        // when no arc point has been picked up
        if (!arcStartPoint || !arcEndPoint || !rotationAxis /*Investigate this*/) return rotationMatrix;

        if (mode == Mode::Drag) {
            rotationAxis = std::make_unique<common::Vector3D>(*arcStartPoint * *arcEndPoint);
            theta = asin(rotationAxis->length() / (arcStartPoint->length() * arcEndPoint->length()));
            rotationMatrix = common::RotationMatrix {rotationAxis->normalize(), math3d::Utilities::asDegrees(theta)};
        } else if (mode == Mode::Scroll) {
            auto currentRotationMatrix = common::RotationMatrix{*rotationAxis, positiveRotation ? 1.f : -1.f};
            rotationMatrix = currentRotationMatrix * rotationMatrix;
        }
        return rotationMatrix;
    }

    math3d::types::Point3D ArcballController::convertCursorToCameraCoordinates(common::Point3D const& cursorPositionDevice) {
        auto cursorPositionCamera = inverseProjectionMatrix * cursorPositionDevice;
        return { cursorPositionCamera.x, cursorPositionCamera.y, cursorPositionCamera.z };
    }

    void ArcballController::setVisualizationOn() {
        visualizationOn = true;
    }

    void ArcballController::setVisualizationOff() {
        visualizationOn = false;
    }

    void ArcballController::createVisualization() {
        visualizationItems.clear();
        visualizationItems.emplace_back(std::make_unique<ArcballSphere>(displayDimensions));

        arcStartPointVisual = std::ref(*(visualizationItems.emplace_back(
                std::make_unique<ArcballPoint>(displayDimensions)).get()));
        arcEndPointVisual = std::ref(*(visualizationItems.emplace_back(
                std::make_unique<ArcballPoint>(displayDimensions)).get()));

        arcStartVectorVisual = std::ref(*(visualizationItems.emplace_back(
                std::make_unique<ArcballDirectionVector>(displayDimensions)).get()));
        arcEndVectorVisual = std::ref(*(visualizationItems.emplace_back(
                std::make_unique<ArcballDirectionVector>(displayDimensions)).get()));

        arcAxisVectorVisual = std::ref(*(visualizationItems.emplace_back(
                std::make_unique<ArcballDirectionVector>(displayDimensions)).get()));


        for (auto& visualizationItem : visualizationItems) {
            visualizationItem->setProjectionMatrix(projectionMatrix);
        }
        // TODO: Read from config
        arcStartVectorVisual->get().setColor({0.f, .7f, 0.f, .9f});
        arcEndVectorVisual->get().setColor({0.f, .7f, 0.f, .9f});
        arcAxisVectorVisual->get().setColor({0.f, 0.f, 0.6f, .9f});
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

    void ArcballController::notifyDisplayResized(common::DisplayDimensions const& displaySize) {
        Renderable::notifyDisplayResized(displaySize);
        displayDimensions = displaySize;
        float height = 1.f;
        float width = height * aspectRatio;
        if (!projectionMatrix) {
            projectionMatrix = std::make_shared<math3d::OrthographicProjectionMatrix<float>>();
        }
        projectionMatrix->update({{-width, -height, -1.f}, {width, height, 1.f}});
        inverseProjectionMatrix = projectionMatrix->inverse();
        for(auto& visualizationItem : visualizationItems) {
            visualizationItem->notifyDisplayResized(displaySize);
            visualizationItem->setProjectionMatrix(projectionMatrix);
        }
    }

}
