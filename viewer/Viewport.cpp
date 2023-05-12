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
    , fogEnabled(false)
    , windowDimensions{} {
        registerEventHandlers();
    }

    Viewport::Viewport(Viewport::ViewportCoordinates  coordinates)
    : coordinates(std::move(coordinates))
    , showGradientBackground(true)
    , fogEnabled(false)
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
        renderableObjects.insert(std::ref(renderable));
    }

    void Viewport::remove(mv::Renderable& renderable) {
        auto itr = renderableObjects.find(std::ref(renderable));
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

    void Viewport::notifyWindowResized(unsigned int const windowWidth, unsigned int const windowHeight) {
        for (auto& renderable : renderableObjects) {
            renderable.get().notifyWindowResized(windowWidth, windowHeight);
        }
        windowDimensions = {windowWidth, windowHeight};
    }

    void Viewport::render() {
        glCallWithErrorCheck(glViewport, coordinates.bottomLeft.x * windowDimensions.width,
                                         coordinates.bottomLeft.y * windowDimensions.height,
                                         (coordinates.topRight.x - coordinates.bottomLeft.x) * windowDimensions.width,
                                         (coordinates.topRight.y - coordinates.bottomLeft.y) * windowDimensions.height);

        // Background has to be drawn first (it's render method will disable writing to depth buffer)
        if (showGradientBackground) {
            if (!gradientBackground) {
                gradientBackground = std::make_unique<objects::GradientBackground>();
            }
            gradientBackground->render();
        }

        // Add fog if enabled
        fogEnabled ? enableFog() : disableFog();

        for (auto& renderable : renderableObjects) {
            renderable.get().render();
        }

    }

    void Viewport::enableFog() {
        for (auto& renderable : renderableObjects) {
            auto shaderProgram = renderable.get().getShaderProgram();
            glCallWithErrorCheck(glUseProgram, shaderProgram);
            GLint fogEnabledId = glGetUniformLocation(shaderProgram, "fog.enabled");
            glCallWithErrorCheck(glUniform1i, fogEnabledId, true);
            GLint fogColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.color");
            //TODO: Read from config
            //TODO: Sample background color
            float fogColor[3] = {0.75f, 0.75f, 0.75f};
            glUniform3fv(fogColorId, 1, fogColor);
            GLint fogMinDistId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.minimumDistance");
            glCallWithErrorCheck(glUniform1f, fogMinDistId, 0.f);
            GLint fogMaxDistId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.maximumDistance");
            glCallWithErrorCheck(glUniform1f, fogMaxDistId, 30.0f);
        }
    }

    void Viewport::disableFog() {
        for (auto& renderable : renderableObjects) {
            if (renderable.get().isReadyToRender()) {
                auto shaderProgram = renderable.get().getShaderProgram();
                glCallWithErrorCheck(glUseProgram, shaderProgram);
                GLint fogEnabledId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "fog.enabled");
                glCallWithErrorCheck(glUniform1i, fogEnabledId, false);
            }
        }
    }

}
