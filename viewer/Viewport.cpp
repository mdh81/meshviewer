#include "Viewport.h"
#include <functional>
#include <utility>
#include <vector>
#include "EventHandler.h"
#include "CallbackFactory.h"
#include "GradientBackground.h"

using namespace mv::events;
using namespace mv::common;

namespace mv::scene {

    Viewport::Viewport(std::initializer_list<float> const& coordinates)
    : coordinates(coordinates)
    , showGradientBackground(true)
    , addFog(false)
    , windowDimensions{} {
        registerEventHandlers();
    }

    Viewport::Viewport(Viewport::ViewportCoordinates  coordinates)
    : coordinates(std::move(coordinates))
    , showGradientBackground(true)
    , addFog(false)
    , windowDimensions{} {
        registerEventHandlers();
    }

    void Viewport::registerEventHandlers() {

        EventHandler().registerCallback(
                Event(GLFW_KEY_N),
                CallbackFactory::getInstance().registerCallback
                        (*this, &Viewport::toggleGlyphsDisplay));

        EventHandler().registerCallback(
                Event(GLFW_KEY_G),
                CallbackFactory::getInstance().registerCallback
                        (*this, &Viewport::toggleGradientBackgroundDisplay));

        EventHandler().registerCallback(
                Event(GLFW_KEY_F),
                CallbackFactory::getInstance().registerCallback
                        (*this, &Viewport::toggleFog));
    }

    void Viewport::add(mv::Renderable& renderable) {
        if (!gradientBackground && dynamic_cast<objects::GradientBackground*>(&renderable)) {
            gradientBackground = std::ref(renderable);
        } else {
            renderableObjects.insert(std::ref(renderable));
        }
    }

    void Viewport::remove(mv::Renderable& renderable) {
        auto itr = renderableObjects.find(std::ref(renderable));
        if (itr != renderableObjects.end()) {
            renderableObjects.erase(itr);
        } else if (gradientBackground && gradientBackground->get() == renderable) {
            gradientBackground.reset();
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

    void Viewport::notifyWindowResized(unsigned int const windowWidth, unsigned int const windowHeight) {
        for (auto& renderable : renderableObjects) {
            renderable.get().notifyWindowResized(windowWidth, windowHeight);
        }
#ifdef OSX
        // OSX retina displays have twice as many pixels
        windowDimensions = {2*windowWidth, 2*windowHeight};
#else
        windowDimensions = {windowWidth, windowHeight};
#endif
    }

    void Viewport::render() {
        glCallWithErrorCheck(glViewport, coordinates.bottomLeft.x * windowDimensions.width,
                                         coordinates.bottomLeft.y * windowDimensions.height,
                                         (coordinates.topRight.x - coordinates.bottomLeft.x) * windowDimensions.width,
                                         (coordinates.topRight.y - coordinates.bottomLeft.y) * windowDimensions.height);

        // Background has to be drawn first (it's render method will disable writing to depth buffer)
        if (gradientBackground && showGradientBackground) {
            gradientBackground->get().render();
        }
        for (auto& renderable : renderableObjects) {
            renderable.get().render();
        }

    }

}
