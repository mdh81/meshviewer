#pragma once

#include "MeshViewerObject.h"
#include "Types.h"
#include "glm/glm.hpp"

namespace mv {

// A renderable is an object that has a visual representation on the screen
class Renderable : public MeshViewerObject {
public:
    using RenderablePointer = std::shared_ptr<Renderable>;
    using Renderables = std::vector<RenderablePointer>;

public:
    virtual void render() = 0;

    [[nodiscard]] float getAspectRatio() const {
        return aspectRatio;
    }

    [[nodiscard]] bool isReadyToRender() const {
        return readyToRender;
    }

    [[nodiscard]]
    virtual common::Point3D getCentroid() const = 0;

    [[nodiscard]]
    virtual common::Bounds getBounds() const = 0;

    virtual void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) {
        aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
        // TODO: This should trigger just projection re-computation not a geometry regenerate
        readyToRender = false;
    }

    virtual void writeToFile(std::string const& fileName, glm::mat4 const& transform) const {
        std::cerr << __PRETTY_FUNCTION__ << " is not implemented" << std::endl;
    }

protected:
    bool readyToRender{};
    float aspectRatio{1.0f};
};

}
