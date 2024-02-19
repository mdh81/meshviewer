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
    , visualizationTTL(500)
    , interactionState {InteractionState::Stopped}
    , mode {Mode::Inactive} {


    }

    void ArcballController::monitorInteraction() {

        while(true) {
            auto currentInteractionTimePoint = std::chrono::high_resolution_clock::now();
            auto elapsedTime = currentInteractionTimePoint - previousInteractionTimePoint;
            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() > visualizationTTL.count()) {
                setVisualizationOff();
                interactionMonitorThread = nullptr;
                arcStartPoint = nullptr;
                arcEndPoint = nullptr;
                mode = Mode::Inactive;
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds (300));
            }
        }

    }

    void ArcballController::reset() {
        arcStartPoint = nullptr;
        arcEndPoint = nullptr;
        positiveRotation = true;
        mode = Mode::Inactive;
    }

    void ArcballController::handleScrollEvent(common::Point3D const& cursorPositionDevice, bool const directionChanged) {

        if (!arcStartPoint) {
            arcStartPoint = getCursorLocationOnArcball(cursorPositionDevice);
            //interactionMonitorThread = std::make_unique<std::thread>(&ArcballController::monitorInteraction, this);
            //interactionMonitorThread->detach();
        } else {
            if (!arcEndPoint) {
                arcEndPoint = getCursorLocationOnArcball(cursorPositionDevice);
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
        }
        mode = Mode::Scroll;
    }

    common::UniquePointer<common::Point3D> ArcballController::getCursorLocationOnArcball(common::Point3D const& cursorPositionDevice) {
        math3d::types::Point3D cursorPositionCamera = convertCursorToCameraCoordinates(cursorPositionDevice);
        // In the camera space the ray from the cursor position is always in the negative z-direction
        auto intersectionResult = sphere.intersectWithRay({cursorPositionCamera, {0, 0, -1}});
        if (intersectionResult.status == math3d::IntersectionStatus::Intersects) {
            return std::make_unique<common::Point3D>(Util::asFloat(intersectionResult.intersectionPoint));
        } else {
            return nullptr;
        }
    }

    void ArcballController::updateVisualization() {
        if (arcStartPoint && originVisual) {
            originVisual->get().setPosition({0.f, 0.f, 0.f});
            arcStartVectorVisual->get().setPosition(*arcStartPoint);
            arcEndVectorVisual->get().setPosition(*arcEndPoint);
            arcAxisVectorVisual->get().setPosition(rotationAxis->normalize());
        }
    }

    common::RotationMatrix ArcballController::getRotation() {

        if (!arcStartPoint || !arcEndPoint) return {};

        if (mode == Mode::Drag) {
            rotationAxis = std::make_unique<common::Vector3D>(*arcStartPoint * *arcEndPoint);
            theta = asin(rotationAxis->length() / (arcStartPoint->length() * arcEndPoint->length()));
        }

        updateVisualization();

        return common::RotationMatrix {rotationAxis->normalize(), math3d::Utilities::asDegrees(theta)};
    }


    common::RotationMatrix ArcballController::getRotation(common::Point3D const& cursorPositionDevice) {
        if (!projectionMatrix) throw std::logic_error("Projection matrix should have been created by this point in time");
        recordCursorPosition(cursorPositionDevice);
        if (!arcEndPoint) {
            return {};
        }
        rotationAxis = std::make_unique<common::Vector3D>(*arcStartPoint * *arcEndPoint);
        std::cout << "Rotation Axis: " << *rotationAxis << "\nRotation amount = " << math3d::Utilities::asDegrees(theta) << std::endl;
        theta = asin(rotationAxis->length() / (arcStartPoint->length() * arcEndPoint->length()));
        previousInteractionTimePoint = std::chrono::high_resolution_clock::now();
        return common::RotationMatrix {rotationAxis->normalize(), math3d::Utilities::asDegrees(theta)};
    }

    math3d::types::Point3D ArcballController::convertCursorToCameraCoordinates(common::Point3D const& cursorPositionDevice) {
        auto cursorPositionCamera = inverseProjectionMatrix * cursorPositionDevice;
        return { cursorPositionCamera.x, cursorPositionCamera.y, cursorPositionCamera.z };
    }

    void ArcballController::recordCursorPosition(common::Point3D const& cursorPositionDevice) {
        math3d::types::Point3D cursorPositionCamera = convertCursorToCameraCoordinates(cursorPositionDevice);
        // In the camera space the ray from the cursor position is always in the negative z-direction
        auto intersectionResult = sphere.intersectWithRay({cursorPositionCamera, {0, 0, -1}});
        if (!arcStartPoint) {
            arcStartPoint = std::make_unique<common::Point3D>(Util::asFloat(intersectionResult.intersectionPoint));
        } else {
            arcEndPoint = std::make_unique<common::Point3D>(Util::asFloat(intersectionResult.intersectionPoint));
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
        visualizationItems.emplace_back(std::make_unique<ArcballSphere>(displayDimensions));
        originVisual = std::ref(*(visualizationItems.emplace_back(std::make_unique<ArcballPoint>(displayDimensions)).get()));
        arcStartVectorVisual = std::ref(*(visualizationItems.emplace_back(std::make_unique<ArcballDirectionVector>(displayDimensions)).get()));
        arcEndVectorVisual = std::ref(*(visualizationItems.emplace_back(std::make_unique<ArcballDirectionVector>(displayDimensions)).get()));
        arcAxisVectorVisual = std::ref(*(visualizationItems.emplace_back(std::make_unique<ArcballDirectionVector>(displayDimensions)).get()));
        for (auto& visualizationItem : visualizationItems) {
            visualizationItem->setProjectionMatrix(projectionMatrix);
        }
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
