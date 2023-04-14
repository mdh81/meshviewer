#pragma once

#include "Viewport.h"
#include <memory>
#include <vector>

namespace mv::scene {

    class Scene {
    public:
        using ViewportPointer = std::unique_ptr<Viewport>;
        using ViewportCollection = std::vector<ViewportPointer>;

    public:
        Scene(unsigned const windowWidth, unsigned const windowHeight);
        ~Scene() = default;
        void createViewport(Viewport::ViewportCoordinates const& coordinates);
        [[nodiscard]]
        ViewportCollection const& getViewports() const { return viewportCollection; }

    private:
        ViewportCollection viewportCollection;
        unsigned const windowWidth;
        unsigned const windowHeight;
    };

}
