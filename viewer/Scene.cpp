#include "Scene.h"
#include "Viewer.h"
#include "GradientBackground.h"
#include <utility>

namespace mv::scene {

Scene::Scene(int frameBufferWidth, int frameBufferHeight) {
    createViewport({{0, 0}, {1, 1}});
    notifyWindowResized(frameBufferWidth, frameBufferHeight);
}

void Scene::createViewport(Viewport::ViewportCoordinates const& coordinates) {
   viewports.emplace_back(new Viewport(coordinates));
}

void Scene::add(Drawable& drawable) {
    viewports.at(0)->add(drawable);
}

void Scene::render() {
    for(auto& viewport : viewports) {
        viewport->render();
    }
}

void Scene::notifyWindowResized(unsigned int const newWindowWidth, unsigned int const newWindowHeight) {
    for(auto& viewport : viewports) {
        viewport->notifyWindowResized(newWindowWidth, newWindowHeight);
    }
}

void Scene::removeViewport(ViewportPointer const& viewportToRemove) {
    ViewportCollection::iterator removedViewport = viewports.end();
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