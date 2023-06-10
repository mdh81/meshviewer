#pragma once

#include "Viewport.h"
#include <memory>
#include <vector>

namespace mv::scene {

// A scene is a collection of viewports
class Scene : public Renderable {
    public:
        using ViewportPointer = std::unique_ptr<Viewport>;
        using ViewportCollection = std::vector<ViewportPointer>;

    public:
        Scene(unsigned frameBufferWidth, unsigned frameBufferHeight);
        ~Scene() = default;
        // Scenes cannot be copied or assigned
        Scene(Scene const&) = delete;
        Scene& operator=(Scene const&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        void createViewport(Viewport::ViewportCoordinates const& coordinates);

        void removeViewport(ViewportPointer const& viewportToRemove);

        [[nodiscard]]
        ViewportCollection const& getViewports() const {
            return viewports;
        }

        // Add a renderable to the default viewport
        void add(Drawable& drawable);

        // Render this scene. Calls render on all its viewports and the renderables in them
        void render() override;

        // Notify this scene that the window was resized
        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) override;

        [[nodiscard]]
        common::Point3D getCentroid() const override {
            return {};
        }

        [[nodiscard]]
        common::Bounds getBounds() const override {
            return {};
        }

private:
        ViewportCollection viewports;
    };

}
