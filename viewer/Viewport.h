#pragma once

#include "Drawable.h"
#include "GradientBackground.h"
#include "3dmath/Vector.h"
#include "Drawable3D.h"
#include "Camera.h"
#include "3dmath/Matrix.h"
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
        math3d::Vector2D<float> getOrigin() const {
            return {coordinates.x.min, coordinates.y.min};
        }

        [[nodiscard]]
        float getWidth() const { return coordinates.x.max - coordinates.x.min; }

        [[nodiscard]]
        float getHeight() const { return coordinates.y.max - coordinates.y.min; }

        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) override;

        [[nodiscard]]
        common::Point3D getCentroid() const override;

        [[nodiscard]]
        common::Bounds getBounds() const override;

        [[nodiscard]]
        math3d::Matrix<float, 3, 3> getWindowToViewportTransform() const {
            return math3d::Matrix<float, 3, 3> {
                    {1.f/windowDimensions.width,  0.f,                         0.f},
                    {0.f,                        -1.f/windowDimensions.height, 1.f},
                    {0.f,                         0.f,                         0.f}
            };
        }

        void writeToFile(std::string const& fileName, glm::mat4 const& transform) const override {
            for (auto& drawable : drawables) {
                if (drawable.get().is3D()) {
                    auto& drawable3D = dynamic_cast<Drawable3D&>(drawable.get());
                    drawable3D.writeToFile(fileName, transform);
                }
            }
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
        void enableFog();
        void disableFog();
        void zoom3DView();
        void pan3DView();
        [[nodiscard]] bool isViewportEvent(common::Point2D const& cursorPosition) const;

    private:
        using DrawablesSet = std::unordered_set<Drawable::DrawableReference,
                MeshViewerObject::MeshViewerObjectHash,
                MeshViewerObject::MeshViewerObjectEquals>;
        DrawablesSet drawables;
        std::unique_ptr<mv::objects::GradientBackground> gradientBackground;
        std::optional<Drawable::DrawableReference> activeObject;
        common::Bounds coordinates;
        common::WindowDimensions windowDimensions;
        Camera::SharedCameraPointer camera;
        bool showGradientBackground;
        bool fogEnabled;
        friend class ViewportTest;
    };
}
