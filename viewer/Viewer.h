#pragma once
#include "Drawable.h"
#include "PointerTypes.h"

namespace mv::viewer {

    // A viewer is a collection of scenes
    class Viewer {
        public:
            // Add drawables to the scene
            virtual void add(Drawable::DrawablePointer&& drawable) = 0;
            virtual void add(Drawable::DrawablePointers const& newDrawables) = 0;
            virtual void add(Drawable::DrawablePointer const&) = 0;

            // Remove a drawable from the scene
            virtual void remove(Drawable::DrawablePointer const&) = 0;

            // Render scene, viewports and renderables
            virtual void render() = 0;

            [[nodiscard]]
            virtual math3d::Matrix<float, 3, 3> getViewportToWindowTransform() const = 0;
    };

}