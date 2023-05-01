#pragma once

#include "../objects/Renderable.h"
#include "3dmath/Vector.h"
#include <unordered_set>

namespace mv::scene {

class Viewport : public MeshViewerObject {
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

        void add(Renderable& renderable);

        void remove(Renderable& renderable);

        void render();

        [[nodiscard]]
        Renderables getRenderables() const;

        [[nodiscard]]
        math3d::Vector2D<float> getOrigin() const {
            return {coordinates.bottomLeft.x, coordinates.bottomLeft.y};
        }

        [[nodiscard]]
        float getWidth() const { return coordinates.topRight.x - coordinates.bottomLeft.x; }

        [[nodiscard]]
        float getHeight() const { return coordinates.topRight.y - coordinates.bottomLeft.y; }

        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight);

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
            addFog = !addFog;
        }
    private:
        RenderableReferences renderableObjects;
        std::optional<RenderableReference> gradientBackground;
        std::optional<RenderableReference> activeObject;
        ViewportCoordinates coordinates;
        common::WindowDimensions windowDimensions;
        bool showGradientBackground;
        bool addFog;
    };
}
