#include "Scene.h"
#include "Viewer.h"
#include "GradientBackground.h"
#include <utility>

namespace mv::scene {

Scene::Scene(int frameBufferWidth, int frameBufferHeight) {
    createViewport({0, 0, 1, 1});
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

}