#include "Scene.h"

#include <utility>

namespace mv::scene {

    Scene::Scene(unsigned const windowWidth, unsigned const windowHeight)
    : windowWidth(windowWidth)
    , windowHeight(windowHeight) {
        createViewport({0, 0, 1, 1});
    }

    void Scene::createViewport(Viewport::ViewportCoordinates const& coordinates) {
       viewportCollection.emplace_back(new Viewport(coordinates));
    }

}