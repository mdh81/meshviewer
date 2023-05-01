#include "Scene.h"
#include "Viewer.h"
#include "GradientBackground.h"

#include <utility>

namespace mv::scene {

Scene::Scene(unsigned const windowWidth, unsigned const windowHeight)
: windowWidth(windowWidth)
, windowHeight(windowHeight) {
    createViewport({0, 0, 1, 1});
}

void Scene::createViewport(Viewport::ViewportCoordinates const& coordinates) {
   viewports.emplace_back(new Viewport(coordinates));
   auto background = new objects::GradientBackground;
   Viewer::getInstance().add(background);
   add(*background);
}

void Scene::add(Renderable& renderable) {
    viewports.at(0)->add(renderable);
}

void Scene::render() {
    for(auto& viewport : viewports) {
        viewport->render();
    }
}

void Scene::notifyWindowResized(unsigned int const windowWidth, unsigned int const windowHeight) {
    for(auto& viewport : viewports) {
        viewport->notifyWindowResized(windowWidth, windowHeight);
    }
}
}