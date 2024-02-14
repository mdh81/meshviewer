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

    ArcballController::ArcballController()
    : Renderable()
    , sphere({0.f, 0.f, 0.f}, 1.f)
    , visualizationTTL(500)
    , interactionState {InteractionState::Stopped} {

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
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds (300));
            }
        }

    }

    void ArcballController::handleScrollEvent(common::Point3D const& cursorPositionDevice) {

        if (!arcStartPoint) {
            // 1. Convert cursor position to sphere point
            // 2. Record arc start point
            // 3. Start timer. Timer will count down to gesture timeout and will reset the counter if another gesture event
            //    were to happen before the timeout
            // 4. When timer runs out, the timer thread will set arcStartPoint and arcEndPoint to nullptr and will fade out the
            //    arc and other visuals

            arcStartPoint = getCursorLocationOnArcball(cursorPositionDevice);
            //interactionMonitorThread = std::make_unique<std::thread>(&ArcballController::monitorInteraction, this);
            //interactionMonitorThread->detach();


        } else {
            arcEndPoint = getCursorLocationOnArcball(cursorPositionDevice);
            // 1. Convert cursor position to sphere point
            // 2. Record arc end point
            if (!arcEndPoint) {
                std::cout << "End point not on arcball" << std::endl;
            }
        }

    }

    std::unique_ptr<common::Point3D> ArcballController::getCursorLocationOnArcball(common::Point3D const& cursorPositionDevice) {
        math3d::types::Point3D cursorPositionCamera = convertCursorToCameraCoordinates(cursorPositionDevice);
        // In the camera space the ray from the cursor position is always in the negative z-direction
        auto intersectionResult = sphere.intersectWithRay({cursorPositionCamera, {0, 0, -1}});
        if (intersectionResult.status == math3d::IntersectionStatus::Intersects) {
            return std::make_unique<common::Point3D>(Util::asFloat(intersectionResult.intersectionPoint));
        } else {
            return nullptr;
        }
    }

    common::RotationMatrix ArcballController::getRotation(common::Point3D const& cursorPositionADevice, common::Point3D const& cursorPositionBDevice) {
        if (!projectionMatrix) throw std::logic_error("Projection matrix should have been created by this point in time");

        common::RotationMatrix rotationMatrix{};
        arcStartPoint = getCursorLocationOnArcball(cursorPositionADevice);
        if (arcStartPoint) {
            arcEndPoint = getCursorLocationOnArcball(cursorPositionBDevice);
            if (arcEndPoint) {
                rotationAxis = std::make_unique<common::Vector3D>(*arcStartPoint * *arcEndPoint);
                //std::cout << "Rotation Axis: " << *rotationAxis / rotationAxis->length() << "\nRotation amount = "
                //          << math3d::Utilities::asDegrees(theta) << std::endl;
                theta = asin(rotationAxis->length() / (arcStartPoint->length() * arcEndPoint->length()));
                rotationMatrix = common::RotationMatrix{rotationAxis->normalize(), math3d::Utilities::asDegrees(theta)};
                //std::cout << "Rotation Matrix: \n" << rotationMatrix << std::endl;
            }
        }
        previousInteractionTimePoint = std::chrono::high_resolution_clock::now();

        if (!interactionMonitorThread) {
            interactionMonitorThread = std::make_unique<std::thread>(&ArcballController::monitorInteraction, this);
            interactionMonitorThread->detach();
        }

        updateVisualization();

        return rotationMatrix;
    }

    void ArcballController::updateVisualization() {
        if (arcStartPoint && originVisual) {
            originVisual->get().setPosition({0.f, 0.f, 0.f});
            arcStartVectorVisual->get().setPosition(*arcStartPoint);
            arcEndVectorVisual->get().setPosition(*arcEndPoint);
            arcAxisVectorVisual->get().setPosition((*arcStartPoint * *arcEndPoint).normalize());
        }
    }

    common::RotationMatrix ArcballController::getRotation() {

        if (!arcStartPoint || !arcEndPoint) return {};

        rotationAxis = std::make_unique<common::Vector3D>(*arcStartPoint * *arcEndPoint);
        std::cout << "Rotation Axis: " << *rotationAxis << "\nRotation amount = " << math3d::Utilities::asDegrees(theta) << std::endl;
        theta = asin(rotationAxis->length() / (arcStartPoint->length() * arcEndPoint->length()));

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
