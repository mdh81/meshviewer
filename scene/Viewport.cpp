#include "Viewport.h"
#include <functional>
#include <vector>

namespace mv::scene {

    Viewport::Viewport(std::initializer_list<float> const& coordinates)
    : coordinates(coordinates) {
    }

    Viewport::Viewport(Viewport::ViewportCoordinates const& viewportCoordinates) :
            coordinates(viewportCoordinates) {

    }

    void Viewport::add(mv::Renderable const &renderable) {
        renderableObjects.insert(std::cref(renderable));
    }

    void Viewport::remove(mv::Renderable const &renderable) {
        auto itr = renderableObjects.find(std::cref(renderable));
        if (itr != renderableObjects.end()) {
            renderableObjects.erase(itr);
        } else {
            throw std::runtime_error("Unable to remove renderable. It was never added");
        }
    }

    Renderables Viewport::getRenderables() const {
        Renderables renderableList;
        renderableList.reserve(renderableObjects.size());
        std::for_each(renderableObjects.begin(), renderableObjects.end(),
                      [&renderableList](auto& renderable) {
            renderableList.push_back(renderable);
        });
        return renderableList;
    }

}
