#pragma once

#include "Viewport.h"
#include <memory>
#include <vector>

namespace mv::scene {

// A scene is a collection of viewports

// TODO: Add unit tests for testing life-cycle of scenes, renderables, and viewports
class Scene : public MeshViewerObject {
    public:
        using ViewportPointer = std::unique_ptr<Viewport>;
        using ViewportCollection = std::vector<ViewportPointer>;

    public:
        Scene(unsigned windowWidth, unsigned windowHeight);
        ~Scene() = default;

        void createViewport(Viewport::ViewportCoordinates const& coordinates);

        [[nodiscard]]
        ViewportCollection const& getViewports() const { return viewports; }

        // Add a renderable to the default viewport
        void add(Renderable& renderable);

        // Render this scene. Calls render on all its viewports and the renderables in them
        void render();

        // Notify this scene that the window was resized
        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight);

    private:
        ViewportCollection viewports;
        unsigned const windowWidth;
        unsigned const windowHeight;
    };

}
