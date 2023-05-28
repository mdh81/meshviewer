#pragma once

#include "Viewport.h"
#include <memory>
#include <vector>

namespace mv::scene {

// A scene is a collection of viewports

// TODO: Add unit tests for testing life-cycle of scenes, renderables, and viewports
class Scene : public Renderable {
    public:
        using ViewportPointer = std::unique_ptr<Viewport>;
        using ViewportCollection = std::vector<ViewportPointer>;

    public:
        // TODO: Complete creation semantics
        Scene(int frameBufferWidth, int frameBufferHeight);
        ~Scene() = default;

        void createViewport(Viewport::ViewportCoordinates const& coordinates);

        [[nodiscard]]
        ViewportCollection const& getViewports() const { return viewports; }

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
