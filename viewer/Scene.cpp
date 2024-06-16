#include "Scene.h"
#include "Viewer.h"
#include "GradientBackground.h"
#include "Types.h"
#include <utility>
#include <algorithm>

namespace mv::scene {

Scene::Scene(common::DisplayDimensions const& displayDimensions) {
    if (!displayDimensions.frameBufferWidth || !displayDimensions.frameBufferHeight) {
        throw std::runtime_error("Scene's dimensions cannot be zero");
    }
    createViewport({{0, 0}, {1, 1}});
    notifyDisplayResized(displayDimensions);
}

void Scene::createViewport(Viewport::ViewportCoordinates const& coordinates) {
   viewports.emplace_back(new Viewport(coordinates));
}

void Scene::add(Drawable& drawable) {
    viewports.at(0)->add(drawable);
}

void Scene::remove(Drawable& drawable) {
    viewports.at(0)->remove(drawable);
}

void Scene::render() {
    for(auto& viewport : viewports) {
        viewport->render();
    }
}

void Scene::notifyDisplayResized(common::DisplayDimensions const& displaySize) {
    for(auto& viewport : viewports) {
        viewport->notifyDisplayResized(displaySize);
    }
}

void Scene::removeViewport(ViewportPointer const& viewportToRemove) {
    auto removedViewport = viewports.end();
    if (viewports.size() > 1) {
        removedViewport =
                std::remove_if(viewports.begin(), viewports.end(),
                               [&viewportToRemove](ViewportPointer &viewport) { return viewport == viewportToRemove; });
    } else if (viewports.at(0) == viewportToRemove) {
        removedViewport = viewports.begin();
    }
    if (removedViewport != viewports.end()) {
        viewports.erase(removedViewport, viewports.end());
    } else {
        throw std::runtime_error("Viewport not found. Instance id = " +
                                 (viewportToRemove ? std::to_string(viewportToRemove->getId()) : std::string("Null")));
    }
}

}
