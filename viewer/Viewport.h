#pragma once

#include "Drawable.h"
#include "GradientBackground.h"
#include "3dmath/Vector.h"
#include "Drawable3D.h"
#include "Camera.h"
#include <unordered_set>


namespace mv::scene {

// A viewport draws a list of drawables. Viewports maintain a reference to drawables that are owned by the Viewer
class Viewport : public Renderable {
    public:
        struct ViewportCoordinates {
            math3d::Vector2D<float> bottomLeft;
            math3d::Vector2D<float> topRight;
            ViewportCoordinates(std::initializer_list<float> const& list) {
                if (list.size() != 4) throw std::runtime_error("Unexpected arguments to initialize viewport coordinates");
                bottomLeft.x = data(list)[0];
                bottomLeft.y = data(list)[1];
                topRight.x = data(list)[2];
                topRight.y = data(list)[3];
            }
        };

    public:
        Viewport(std::initializer_list<float> const& viewportCoordinates);

        explicit Viewport(ViewportCoordinates  viewportCoordinates);

        void add(Drawable& drawable);

        void remove(Drawable& drawable);

        void render() override;

        [[nodiscard]]
        Drawable::DrawableReferences getDrawables() const;

        [[nodiscard]]
        math3d::Vector2D<float> getOrigin() const {
            return {coordinates.bottomLeft.x, coordinates.bottomLeft.y};
        }

        [[nodiscard]]
        float getWidth() const { return coordinates.topRight.x - coordinates.bottomLeft.x; }

        [[nodiscard]]
        float getHeight() const { return coordinates.topRight.y - coordinates.bottomLeft.y; }

        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) override;

        [[nodiscard]]
        common::Point3D getCentroid() const override;

        [[nodiscard]]
        common::Bounds getBounds() const override;

        void writeToFile(std::string const& fileName, glm::mat4 const& transform) const override {
            for (auto& drawable : drawables) {
                if (drawable.get().is3D()) {
                    Drawable3D& drawable3D = dynamic_cast<Drawable3D&>(drawable.get());
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
    private:
        using DrawablesSet = std::unordered_set<Drawable::DrawableReference,
                MeshViewerObject::MeshViewerObjectHash,
                MeshViewerObject::MeshViewerObjectEquals>;
        DrawablesSet drawables;
        std::unique_ptr<mv::objects::GradientBackground> gradientBackground;
        std::optional<Drawable::DrawableReference> activeObject;
        ViewportCoordinates coordinates;
        common::WindowDimensions windowDimensions;
        Camera::SharedCameraPointer camera;
        bool showGradientBackground;
        bool fogEnabled;
    };
}
