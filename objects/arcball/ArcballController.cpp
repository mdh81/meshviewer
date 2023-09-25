#include "ArcballController.h"
#include "ArcballSphere.h"
#include "ArcballPoint.h"
#include "Util.h"
#include "3dmath/MatrixOperations.h"
#include "Types.h"
#include <thread>


namespace {
    math3d::Vector3<float> foo;
    math3d::Matrix<float, 4, 4> fooMat4;
    math3d::Matrix<float, 3, 3> fooMat3;

    math3d::Vector3<double> food;
    math3d::Matrix<double, 4, 4> fooMat4d;
    math3d::Matrix<double, 3, 3> fooMat3d;
}

namespace mv::objects {

    ArcballController::ArcballController()
    : Renderable()
    , sphere({0.f, 0.f, 0.f}, 1.f) {

    }

    [[nodiscard]]
    common::RotationMatrix ArcballController::getRotation(common::Point3D const& cursorPositionDevice) {
        if (!projectionMatrix) throw std::logic_error("Projection matrix should have been created by this point in time");
        recordCursorPosition(cursorPositionDevice);
        if (!pointB) {
            return {};
        }
        rotationAxis = std::make_unique<common::Vector3D>(*pointA * *pointB);
        std::cout << "Rotation Axis: " << *rotationAxis << "\nRotation amount = " << math3d::Utilities::asDegrees(theta) << std::endl;
        theta = asin(rotationAxis->length() / (pointA->length() * pointB->length()));
        return common::RotationMatrix {*rotationAxis, math3d::Utilities::asDegrees(theta)};
    }

    math3d::types::Point3D ArcballController::convertCursorToCameraCoordinates(common::Point3D const& cursorPositionDevice) {
        auto cursorPositionCamera = inverseProjectionMatrix * cursorPositionDevice;
        std::cout << "Cursor in camera coordinates: \n" << cursorPositionCamera << std::endl;
        return { cursorPositionCamera.x, cursorPositionCamera.y, cursorPositionCamera.z };
    }

    void ArcballController::recordCursorPosition(common::Point3D const& cursorPositionDevice) {
        math3d::types::Point3D cursorPositionCamera = convertCursorToCameraCoordinates(cursorPositionDevice);
        // In the camera space the ray from the cursor position is always in the negative z-direction
        auto intersectionResult = sphere.intersectWithRay({cursorPositionCamera, {0, 0, -1}});
        if (!pointA) {
            pointA = std::make_unique<common::Point3D>(Util::asFloat(intersectionResult.intersectionPoint));
        } else {
            pointB = std::make_unique<common::Point3D>(Util::asFloat(intersectionResult.intersectionPoint));
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
            visualizationItem->setProjectionMatrix(projectionMatrix);
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

    void ArcballController::notifyDisplayResized(common::DisplayDimensions const& displaySize) {
        Renderable::notifyDisplayResized(displaySize);
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
