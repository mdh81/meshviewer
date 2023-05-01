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

    public:
        Renderable() = delete;
        virtual ~Renderable() = default;
        Renderable(std::string vertexShaderFileName, std::string fragmentShaderFileName);
        void createShaderProgram();
        virtual void render() = 0;

        [[nodiscard]]
        virtual common::Point3D getCentroid() const = 0;

        [[nodiscard]]
        virtual common::Bounds getBounds() const = 0;

        [[nodiscard]]
        Camera& getCamera() { return camera; }

        // TODO: Support model coordinates for 3DRenderable objects
        // TODO: Use 3dmath matrix
        static glm::mat4 getModelTransform() { return glm::mat4(1.0); }

        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) {
            aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
            // TODO: This should trigger just projection re-computation not a geometry regenerate
            m_readyToRender = false;
        }

        float getAspectRatio() const { return aspectRatio; }

        [[nodiscard]] virtual bool supportsGlyphs() { return false; }

        [[nodiscard]] bool isGlyphDisplayOn() const { return glyphsOn; }

        void setGlyphsOn(bool isOn) { glyphsOn = isOn; }

    protected:
        // Set the shader transform matrix inputs
        void setTransforms();

        // Generate data for the rendering pipeline
        virtual void generateRenderData() = 0;

        // Set colors for the rendered object
        virtual void generateColors() = 0;

    protected:
        const std::string m_vertexShaderFileName;
        const std::string m_fragmentShaderFileName;
        unsigned m_shaderProgram{};
        unsigned m_vertexArrayObject{};
        unsigned m_elementBufferObject{};
        bool m_readyToRender;
        float aspectRatio;
        Camera camera;
        bool glyphsOn;
};

using RenderableReference = std::reference_wrapper<Renderable>;
using RenderableReferences = std::unordered_set<RenderableReference,
        MeshViewerObject::MeshViewerObjectHash,
        MeshViewerObject::MeshViewerObjectEquals>;
using Renderables = std::vector<RenderableReference>;

}