#include "Viewport.h"
#include <functional>
#include <vector>
#include "EventHandler.h"
#include "Types.h"
#include "Util.h"
using namespace std::chrono_literals;

namespace mv::scene {

    namespace {
        constexpr auto interactionTTL {200ms};
        constexpr auto interactionThreadPauseInterval {20ms};
    }

    Viewport::Viewport(Viewport::ViewportCoordinates  coordinates)
    : coordinates(coordinates)
    , showGradientBackground(true)
    , fogEnabled(false)
    , displayDimensions{}
    , showArcball(false)
    , arcballController(std::make_unique<objects::ArcballController>()){
        MeshViewerObject::debug = true;
        registerEventHandlers();
    }

    void Viewport::registerEventHandlers() {

        mv::events::EventHandler{}.registerBasicEventCallback(
                GLFW_KEY_N, *this, &Viewport::toggleGlyphsDisplay);

        mv::events::EventHandler().registerBasicEventCallback(
                GLFW_KEY_G, *this, &Viewport::toggleGradientBackgroundDisplay);

        mv::events::EventHandler().registerBasicEventCallback(
                GLFW_KEY_F, *this, &Viewport::toggleFog);

        // TODO: Depending on FPS degradation, it might be better to make these basic events and store the cursor position
        // and cursor position difference in EventHandler and have Viewport grab it from EventHandler or create an
        // intermediary mono-state object like CursorState that can be used by all pieces of the app
        // My feeling is any_cast is slow for gesture events
        mv::events::EventHandler().registerDataEventCallback(
                mv::events::Event{events::EventId::Zoomed}, *this, &Viewport::zoom3DView);

        mv::events::EventHandler().registerDataEventCallback(
                mv::events::Event{events::EventId::Panned}, *this, &Viewport::pan3DView);

        mv::events::EventHandler().registerDataEventCallback(
                mv::events::Event{events::EventId::ScrollRotated}, *this, &Viewport::scrollRotate3DView);

        mv::events::EventHandler().registerDataEventCallback(
                mv::events::Event{events::EventId::DragRotated}, *this, &Viewport::dragRotate3DView);

        mv::events::EventHandler().registerBasicEventCallback(
                mv::events::Event{GLFW_KEY_A, GLFW_MOD_SHIFT}, *this, &Viewport::toggleArcballDisplay);
    }

    void Viewport::add(mv::Drawable& drawable) {
        drawables.insert(std::ref(drawable));
        // Set the camera and display dimensions so the drawable can
        // set the view and projection transforms
        if (camera) drawable.setCamera(camera);
        drawable.notifyDisplayResized(displayDimensions);
    }

    void Viewport::remove(mv::Drawable& drawable) {
        auto itr = drawables.find(std::ref(drawable));
        if (itr != drawables.end()) {
            drawables.erase(itr);
        } else {
            throw std::runtime_error("Unable to remove drawable. It was never added");
        }
    }

    Drawable::DrawableReferences Viewport::getDrawables() const {
        Drawable::DrawableReferences drawableList;
        drawableList.reserve(drawables.size());
        std::for_each(drawables.begin(), drawables.end(),
                      [&drawableList](auto& drawable) {
            drawableList.push_back(drawable);
        });
        return drawableList;
    }

    void Viewport::notifyDisplayResized(common::DisplayDimensions const& displayDimensions) {
        Renderable::notifyDisplayResized(displayDimensions);
        for (auto& drawable : drawables) {
            drawable.get().notifyDisplayResized(displayDimensions);
        }
        this->displayDimensions = displayDimensions;
        this->displayDimensions.normalizedViewportSize = {coordinates.x.max - coordinates.x.min, coordinates.y.max - coordinates.y.min};
        arcballController->notifyDisplayResized(this->displayDimensions);
    }

    void Viewport::render() {
        using namespace mv::common;
        displayDimensions.normalizedViewportSize = {coordinates.x.max - coordinates.x.min, coordinates.y.max - coordinates.y.min};
        glCallWithErrorCheck(glViewport, coordinates.x.min * displayDimensions.frameBufferWidth,
                             coordinates.y.min * displayDimensions.frameBufferHeight,
                             coordinates.x.length() * displayDimensions.frameBufferWidth,
                             coordinates.y.length() * displayDimensions.frameBufferHeight);

        if (!camera) {
            // First render
            camera = std::make_shared<mv::Camera>(*this, Camera::ProjectionType::Perspective);
            for (auto &drawable: drawables) {
                // All 3D drawables share this viewport's camera
                if (drawable.get().is3D()) {
                    drawable.get().setCamera(camera);
                }
                // Set the initial window size for all drawables during this first render call
                drawable.get().notifyDisplayResized(displayDimensions);
            }
            arcballController->notifyDisplayResized(displayDimensions);
        }

        // Compute the view
        camera->apply();

        // Background has to be drawn first (it's render method will disable writing to depth buffer)
        if (showGradientBackground) {
            displayGradientBackground();
        }

        // Add fog if enabled
        fogEnabled ? enableFog() : disableFog();

        for (auto& drawable : drawables) {
            drawable.get().render();
        }

        // Draw arcball interactor next with depth write disabled so all scene objects will render on top of arcball
        // NOTE: render call is always made to support the fade out use case
        arcballController->render();
    }

    void Viewport::displayGradientBackground() {
        if (!gradientBackground) {
            gradientBackground = std::make_unique<objects::GradientBackground>();
        }
        gradientBackground->render();
    }

    void Viewport::enableFog() {
        using namespace mv::common;
        for (auto& drawable : drawables) {
            auto shaderProgram = drawable.get().getShaderProgram();
            glCallWithErrorCheck(glUseProgram, shaderProgram);
            GLint fogEnabledId = glGetUniformLocation(shaderProgram, "fog.enabled");
            glCallWithErrorCheck(glUniform1i, fogEnabledId, true);
            GLint fogColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.color");
            //TODO: Read from config
            //TODO: Sample background color
            float fogColor[3] = {0.75f, 0.75f, 0.75f};
            glUniform3fv(fogColorId, 1, fogColor);
            GLint fogMinDistId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.minimumDistance");
            glCallWithErrorCheck(glUniform1f, fogMinDistId, 0.f);
            GLint fogMaxDistId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.maximumDistance");
            glCallWithErrorCheck(glUniform1f, fogMaxDistId, 30.0f);
        }
    }

    void Viewport::disableFog() {
        using namespace mv::common;
        for (auto& drawable : drawables) {
            if (drawable.get().isReadyToRender()) {
                auto shaderProgram = drawable.get().getShaderProgram();
                glCallWithErrorCheck(glUseProgram, shaderProgram);
                GLint fogEnabledId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.enabled");
                glCallWithErrorCheck(glUniform1i, fogEnabledId, false);
            }
        }
    }

    mv::common::Point3D Viewport::getCentroid() const {
        if (drawables.empty()) {
            //std::cerr << std::format("{}: No drawables were found.", __PRETTY_FUNCTION__);
            return {};
        }
        mv::common::Point3D centroid;
        for (auto& drawable : drawables) {
            auto drawableCentroid = drawable.get().getCentroid();
            centroid.x += drawableCentroid.x;
            centroid.y += drawableCentroid.y;
            centroid.z += drawableCentroid.z;
        }
        centroid.x /= static_cast<float>(drawables.size());
        centroid.y /= static_cast<float>(drawables.size());
        centroid.z /= static_cast<float>(drawables.size());
        return centroid;
    }

    mv::common::Bounds Viewport::getBounds() const {
        if (drawables.empty()) {
            return {};
        }
        mv::common::Bounds viewportBounds;
        for (auto& drawable : drawables) {
            auto drawableBounds = drawable.get().getBounds();
            viewportBounds.x.min = std::min(drawableBounds.x.min, viewportBounds.x.min);
            viewportBounds.y.min = std::min(drawableBounds.y.min, viewportBounds.y.min);
            viewportBounds.z.min = std::min(drawableBounds.z.min, viewportBounds.z.min);

            viewportBounds.x.max = std::max(drawableBounds.x.max, viewportBounds.x.max);
            viewportBounds.y.max = std::max(drawableBounds.y.max, viewportBounds.y.max);
            viewportBounds.z.max = std::max(drawableBounds.z.max, viewportBounds.z.max);
        }
        return viewportBounds;
    }

    void Viewport::monitorInteraction() {
        while(true) {
            auto currentInteractionTimePoint = std::chrono::high_resolution_clock::now();
            auto elapsedTime = currentInteractionTimePoint - previousInteractionTimePoint.load();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() > interactionTTL.count()) {
                scrollGestureStartPosition = {-1, -1}; // Reset scroll start position once the interaction timer expires
                break;
            } else {
                std::this_thread::sleep_for(interactionThreadPauseInterval);
            }
        }
    }

    void Viewport::zoom3DView(events::EventData&& zoomEventData) {
        if (zoomEventData.size() != 2) {
            throw std::runtime_error("Zoom event data is incorrect. Need the current cursor position and the "
                                     "cursor position difference to determine the zoom direction");
        }
        common::Point2D cursorPosition = std::any_cast<common::Point2D>(zoomEventData[0]);
        common::Point2D cursorPositionDifference;
        if (isViewportEvent(cursorPosition)) {
            cursorPositionDifference = std::any_cast<common::Point2D>(zoomEventData[1]);
            cursorPositionDifference.y > 0 ? camera->zoom(common::Direction::Forward) :
                                             camera->zoom(common::Direction::Backward);
        }
    }

    void Viewport::pan3DView(events::EventData&& panEventData) {
        if (panEventData.size() != 2) {
            throw std::runtime_error("Pan event data is incorrect. Need the current cursor position and the "
                                     "cursor position difference to determine the pan direction");
        }
        common::Point2D cursorPosition = std::any_cast<common::Point2D>(panEventData[0]);
        common::Point2D cursorPositionDifference;
        if (isViewportEvent(cursorPosition)) {
            cursorPositionDifference = std::any_cast<common::Point2D>(panEventData[1]);
            if (fabs(cursorPositionDifference.x) > fabs(cursorPositionDifference.y)) {
                camera->pan(cursorPositionDifference.x > 0 ? common::Direction::Right : common::Direction::Left);
            } else {
                camera->pan(cursorPositionDifference.y > 0 ? common::Direction::Down : common::Direction::Up);
            }
        }
    }

    void Viewport::scrollRotate3DView(events::EventData&& rotateEventData) {
        if (rotateEventData.size() != 2) {
            throw std::runtime_error("Rotate event data is incorrect. Need the current cursor position and the "
                                     "cursor position difference to determine the rotation");
        }
        common::Point2D cursorPosition = std::any_cast<common::Point2D>(rotateEventData[0]);
        if (!isViewportEvent(cursorPosition)) return;

        previousInteractionTimePoint = std::chrono::high_resolution_clock::now();
        auto cursorPositionDifference = std::any_cast<common::Point2D>(rotateEventData[1]);

        if (cursorPosition != scrollGestureStartPosition) {
            scrollGestureStartPosition = cursorPosition;
            scrollGesturePreviousPosition = scrollGestureStartPosition;
            arcballController->reset();
            interactionMonitorThread = std::make_unique<std::thread>(&Viewport::monitorInteraction, this);
            interactionMonitorThread->detach();
        }
        auto cursorPositionWithScroll = scrollGesturePreviousPosition + cursorPositionDifference;
        auto cursorPositionViewport = convertWindowToViewportCoordinates(cursorPositionWithScroll);
        scrollGesturePreviousPosition = cursorPositionWithScroll;
        arcballController->handleScrollEvent(
                getViewportToDeviceTransform() * convertWindowToViewportCoordinates(cursorPositionWithScroll),
                scrollDirection && scrollDirection->dot(cursorPositionDifference) < 0);
        camera->setRotation(arcballController->getRotation());
        scrollDirection = cursorPositionDifference;
    }

    void Viewport::dragRotate3DView(events::EventData&& rotateEventData) {
        if (rotateEventData.size() != 1) {
            throw std::runtime_error("Drag rotate event data is incorrect. Need the current cursor position");
        }
        common::Point2D cursorPosition = std::any_cast<common::Point2D>(rotateEventData[0]);
        if (!isViewportEvent(cursorPosition)) return;

        arcballController->handleDragEvent(
                getViewportToDeviceTransform() * convertWindowToViewportCoordinates(cursorPosition));
        camera->setRotation(arcballController->getRotation());
    }

    common::Point2D Viewport::convertWindowToViewportCoordinates(common::Point2D const& windowCoordinates) const {
        auto windowToViewport = getWindowToViewportTransform();
        common::Point3D cursorPositionWindow = {windowCoordinates.x, windowCoordinates.y, 1.f };
        common::Point3D cursorPositionViewport = windowToViewport * cursorPositionWindow;
        return {cursorPositionViewport.x, cursorPositionViewport.y};
    }

    bool Viewport::isViewportEvent(common::Point2D const& cursorPosition) const {
        auto viewportCoordinate = convertWindowToViewportCoordinates(cursorPosition);
        return coordinates.contains({viewportCoordinate.x, viewportCoordinate.y, 0.f});
    }

    math3d::Matrix<float, 3, 3> Viewport::getWindowToViewportTransform() const {
        return math3d::Matrix<float, 3, 3> {
                {getWidth() / displayDimensions.windowWidth, 0.f,                                           0.f},
                {0.f,                                        -getHeight() / displayDimensions.windowHeight, getHeight()},
                {0.f,                                        0.f,                                           0.f}
        };
    }

    // TODO: Add derivation to docs
    [[nodiscard]]
    math3d::Matrix<float, 3, 3> Viewport::getViewportToDeviceTransform() const {
        return math3d::Matrix<float, 3, 3> {
                {2.f / getWidth(), 0.f,                 -1.f},
                {0.f,              2.f / getHeight(),   -1.f},
                {0.f,              0.f,                 -1.f}
        };
    }

}
