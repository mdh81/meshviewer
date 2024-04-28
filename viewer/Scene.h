#pragma once

#include "Viewport.h"
#include "Types.h"
#include <memory>
#include <vector>

namespace mv::scene {

// A scene is a collection of viewports
class Scene : public Renderable {
    public:
        using ViewportPointer = std::unique_ptr<Viewport>;
        using ViewportCollection = std::vector<ViewportPointer>;

    public:
        Scene(common::DisplayDimensions const& displayDimensions);
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
        void notifyDisplayResized(const common::DisplayDimensions &displaySize) override;

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
