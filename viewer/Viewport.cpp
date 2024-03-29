#include "Viewport.h"
#include <functional>
#include <utility>
#include <vector>
#include "EventHandler.h"
#include "Types.h"

namespace mv::scene {

    Viewport::Viewport(Viewport::ViewportCoordinates  coordinates)
    : coordinates(std::move(coordinates))
    , showGradientBackground(true)
    , fogEnabled(false)
    , windowDimensions{} {
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
    }

    void Viewport::add(mv::Drawable& drawable) {
        drawables.insert(std::ref(drawable));
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

    void Viewport::notifyWindowResized(unsigned int const windowWidth, unsigned int const windowHeight) {
        for (auto& drawable : drawables) {
            drawable.get().notifyWindowResized(windowWidth, windowHeight);
        }
        windowDimensions = {windowWidth, windowHeight};
        Renderable::notifyWindowResized(windowWidth, windowHeight);
    }

    void Viewport::render() {
        using namespace mv::common;
        glCallWithErrorCheck(glViewport, coordinates.x.min * windowDimensions.width,
                             coordinates.y.min * windowDimensions.height,
                             coordinates.x.length() * windowDimensions.width,
                             coordinates.y.length() * windowDimensions.height);

        if (!camera) {
            camera = std::make_shared<mv::Camera>(*this, Camera::ProjectionType::Perspective);
            for (auto &drawable: drawables) {
                // All 3D drawables share this viewport's camera
                if (drawable.get().is3D()) {
                    drawable.get().setCamera(camera);
                }
                // Set the initial window size for all drawables during this first render call
                drawable.get().notifyWindowResized(windowDimensions.width, windowDimensions.height);
            }
        }

        // Compute the view
        camera->apply();

        // Background has to be drawn first (it's render method will disable writing to depth buffer)
        if (showGradientBackground) {
            if (!gradientBackground) {
                gradientBackground = std::make_unique<objects::GradientBackground>();
            }
            gradientBackground->render();
        }

        // Add fog if enabled
        fogEnabled ? enableFog() : disableFog();

        for (auto& drawable : drawables) {
            drawable.get().render();
        }

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
            throw std::runtime_error("Cannot compute centroid. Viewport is empty");
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
            throw std::runtime_error("Cannot compute centroid. Viewport is empty");
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

    bool Viewport::isViewportEvent(const common::Point2D& cursorPosition) const {
        auto windowToViewport = getWindowToViewportTransform();
        common::Point3D cursorWindowPosition = { cursorPosition.x, cursorPosition.y, 1.f };
        common::Point3D cursorViewportPosition = windowToViewport * cursorWindowPosition;
        return coordinates.contains(cursorViewportPosition);
    }

}
