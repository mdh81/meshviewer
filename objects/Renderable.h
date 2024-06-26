#pragma once

#include "MeshViewerObject.h"
#include "Types.h"

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

    virtual void notifyDisplayResized(common::DisplayDimensions const& displayDimensions) {
        aspectRatio =
            static_cast<float>(displayDimensions.frameBufferWidth) /
            static_cast<float>(displayDimensions.frameBufferHeight);
        updateProjection = true;
        // TODO: This should trigger just projection re-computation not a geometry regenerate
        readyToRender = false;
    }

    virtual void writeToFile(std::string const& fileName, common::TransformMatrix const& transform) const {
        std::cerr << __PRETTY_FUNCTION__ << " is not implemented" << std::endl;
    }

protected:
    [[nodiscard]] bool needsProjectionUpdate() const {
        return updateProjection;
    }
    void setProjectionUpdated() {
        updateProjection = false;
    }

protected:
    bool readyToRender{};
    bool updateProjection{true};
    float aspectRatio{1.0f};
};

}
