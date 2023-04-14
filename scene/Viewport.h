#pragma once

#include "Renderable.h"
#include "3dmath/Vector.h"
#include <unordered_set>

namespace mv::scene {

    class Viewport {
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
        explicit Viewport(ViewportCoordinates const& viewportCoordinates);
        void add(Renderable const& renderable);
        void remove(Renderable const& renderable);
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
    private:
        RenderableReferences renderableObjects;
        ViewportCoordinates coordinates;
    };
}
