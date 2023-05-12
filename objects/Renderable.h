#pragma once

#include "MeshViewerObject.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "Camera.h"
#include <string>
#include <unordered_set>
#include <vector>

namespace mv {

class Camera;

class Renderable : public MeshViewerObject {

    public:
        // Renderables are meant to be shared across viewports
        // and scenes
        using RenderablePointer = std::shared_ptr<Renderable>;
        using Renderables = std::vector<RenderablePointer>;

        enum Effect {
            None = 0,
            Fog = 1,
        };

    public:
        Renderable() = delete;
        virtual ~Renderable() = default;
        Renderable(std::string vertexShaderFileName, std::string fragmentShaderFileName, Effect supportedEffects = Effect::None);
        void createShaderProgram();
        virtual void render() = 0;

        [[nodiscard]]
        virtual common::Point3D getCentroid() const = 0;

        [[nodiscard]]
        virtual common::Bounds getBounds() const = 0;

        // TODO: 1-1 relationship between camera and renderable is not meaningful. For items like background
        // and axes it makes sense to have separate cameras but not for each 3D renderable. We should have a
        // 1-1 relationship between a viewport and a camera
        [[nodiscard]]
        Camera& getCamera() { return camera; }

        // TODO: Support model coordinates for 3DRenderable objects
        // TODO: Use 3dmath matrix
        static glm::mat4 getModelTransform() { return glm::mat4(1.0); }

        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) {
            aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
            // TODO: This should trigger just projection re-computation not a geometry regenerate
            readyToRender = false;
        }

        [[nodiscard]] unsigned getShaderProgram() const { return shaderProgram; }

        [[nodiscard]] float getAspectRatio() const { return aspectRatio; }

        [[nodiscard]] virtual bool supportsGlyphs() const { return false; }

        [[nodiscard]] bool isGlyphDisplayOn() const { return glyphsOn; }

        [[nodiscard]] bool isReadyToRender() const { return readyToRender; }

        [[nodiscard]] bool supportsEffect(Effect effect) const { return supportedEffects & effect; }

        [[nodiscard]] virtual bool isModelObject() const { return false; }

        void setGlyphsOn(bool isOn) { glyphsOn = isOn; }

    protected:
        // Set the shader transform matrix inputs
        void setTransforms();

        // Generate data for the rendering pipeline
        virtual void generateRenderData() = 0;

        // Set colors for the rendered object
        virtual void generateColors() = 0;

    protected:
        const std::string vertexShaderFileName;
        const std::string fragmentShaderFileName;
        unsigned shaderProgram{};
        unsigned vertexArrayObject{};
        unsigned elementBufferObject{};
        bool readyToRender{};
        float aspectRatio{1.0f};
        Camera camera;
        bool glyphsOn{};
        const unsigned supportedEffects{};
};

using RenderableReference = std::reference_wrapper<Renderable>;
using RenderableReferences = std::unordered_set<RenderableReference,
        MeshViewerObject::MeshViewerObjectHash,
        MeshViewerObject::MeshViewerObjectEquals>;
using Renderables = std::vector<RenderableReference>;

}