#pragma once

#include "Drawable.h"
#include "GradientBackground.h"
#include "3dmath/Vector.h"
#include "Drawable3D.h"
#include "Camera.h"
#include "arcball/ArcballController.h"
#include "3dmath/Vector.h"
#include "3dmath/Matrix.h"
#include "Types.h"
#include "EventTypes.h"
#include <unordered_set>


namespace mv::scene {

// A viewport draws a list of drawables. Viewports maintain a reference to drawables that are owned by the Viewer
class Viewport : public Renderable {
    public:
        using ViewportCoordinates = common::Bounds;

    public:
        explicit Viewport(ViewportCoordinates viewportCoordinates);

        void add(Drawable& drawable);

        void remove(Drawable& drawable);

        void render() override;

        [[nodiscard]]
        Drawable::DrawableReferences getDrawables() const;

        [[nodiscard]]
        common::Vector2D getOrigin() const {
            return {coordinates.x.min, coordinates.y.min};
        }

        [[nodiscard]]
        float getWidth() const { return coordinates.x.max - coordinates.x.min; }

        [[nodiscard]]
        float getHeight() const { return coordinates.y.max - coordinates.y.min; }

        void notifyDisplayResized(const common::DisplayDimensions &displaySize) override;

        [[nodiscard]]
        common::Point3D getCentroid() const override;

        [[nodiscard]]
        common::Bounds getBounds() const override;

        // TODO: Add derivation to docs
        [[nodiscard]]
        math3d::Matrix<float, 3, 3> getWindowToViewportTransform() const {
            return math3d::Matrix<float, 3, 3> {
                {getWidth() / displayDimensions.windowWidth, 0.f,                                           0.f},
                {0.f,                                        -getHeight() / displayDimensions.windowHeight, getHeight()},
                {0.f,                                        0.f,                                           0.f}
            };
        }

        // TODO: Add derivation to docs
        [[nodiscard]]
        math3d::Matrix<float, 3, 3> getViewportToDeviceTransform() const {
            return math3d::Matrix<float, 3, 3> {
                {2.f / getWidth(), 0.f,                 -1.f},
                {0.f,              2.f / getHeight(),   -1.f},
                {0.f,              0.f,                 -1.f}
            };
        }

        void writeToFile(std::string const& fileName, common::TransformMatrix const& transform) const override {
            for (auto& drawable : drawables) {
                if (drawable.get().is3D()) {
                    auto& drawable3D = dynamic_cast<Drawable3D&>(drawable.get());
                    drawable3D.writeToFile(fileName, transform);
                }
            }
        }
        void enableGradientBackground(bool on) {
            showGradientBackground = on;
        }

private:
        void registerEventHandlers();
        void toggleGlyphsDisplay() {
            if (activeObject && activeObject->get().supportsGlyphs()) {
                auto& renderable = activeObject->get();
                renderable.setGlyphsOn(renderable.isGlyphDisplayOn());
            }
        }
        void toggleGradientBackgroundDisplay() {
            showGradientBackground = !showGradientBackground;
        }
        void toggleFog() {
            fogEnabled = !fogEnabled;
        }
        void toggleArcballDisplay() {
            showArcball = !showArcball;
            showArcball ? arcballController->setVisualizationOn() : arcballController->setVisualizationOff();
        }
        void enableFog();
        void disableFog();
        void zoom3DView(events::EventData&&);
        void pan3DView(events::EventData&&);
        void rotate3DViewWithScrollGesture();
        void displayGradientBackground();
        [[nodiscard]]
        bool isViewportEvent(common::Point2D const& cursorPosition) const;
        [[nodiscard]]
        common::Point2D convertWindowToViewportCoordinates(common::Point2D const& windowCoordinates) const;

    private:
        using DrawablesSet = std::unordered_set<Drawable::DrawableReference,
                MeshViewerObject::MeshViewerObjectHash,
                MeshViewerObject::MeshViewerObjectEquals>;
        DrawablesSet drawables;
        std::unique_ptr<mv::objects::GradientBackground> gradientBackground;
        std::unique_ptr<mv::objects::ArcballController> arcballController;
        std::optional<Drawable::DrawableReference> activeObject;
        common::Bounds coordinates;
        common::DisplayDimensions displayDimensions;
        Camera::SharedCameraPointer camera;
        bool showGradientBackground;
        bool fogEnabled;
        bool showArcball;
        common::Point2D scrollGestureStartPosition{};
        common::Point2D scrollGesturePreviousPosition{};
        friend class ViewportTest;
    };
}
