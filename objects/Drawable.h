#pragma once

#include "Renderable.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "Camera.h"
#include <string>
#include <unordered_set>
#include <vector>

namespace mv {

// A drawable is a concrete renderable with geometry that can be drawn in one more scenes or viewports

class Drawable : public Renderable {

    public:
        using DrawableReference = std::reference_wrapper<Drawable>;
        using DrawableReferences = std::vector<DrawableReference>;
        using DrawablePointer = std::shared_ptr<Drawable>;
        using Drawables = std::vector<DrawablePointer>;
        enum Effect {
            None = 0,
            Fog = 1,
        };

    public:
        Drawable(std::string  vertexShaderFileName, std::string  fragmentShaderFileName,
                 Camera::SharedCameraPointer camera, Effect supportedEffects);
        Drawable() = delete;
        virtual ~Drawable() = default;

        [[nodiscard]]
        Camera::SharedCameraPointer getCamera() { return camera; }

        void setCamera(Camera::SharedCameraPointer& newCamera) { camera = newCamera; }

        // TODO: Support model coordinates for 3DRenderable objects
        // TODO: Use 3dmath matrix
        static glm::mat4 getModelTransform() { return glm::mat4(1.0); }

        [[nodiscard]] unsigned getShaderProgram() const { return shaderProgram; }

        [[nodiscard]] bool supportsEffect(Effect effect) const {
            return supportedEffects & effect;
        }

        [[nodiscard]] virtual bool supportsGlyphs() const { return false; }

        [[nodiscard]] bool isGlyphDisplayOn() const { return glyphsOn; }

        void setGlyphsOn(bool isOn) { glyphsOn = isOn; }

        [[nodiscard]] virtual bool is3D() const { return false; }

    protected:
        // Set the shader transform matrix inputs
        void setTransforms();

        // TODO: Does this make sense for a viewport? If so, it can move to Renderable
        // Generate data for the rendering pipeline
        virtual void generateRenderData() = 0;

        // Set colors for the rendered object
        virtual void generateColors() = 0;

        // Create a shader program that can be used to draw the contents of this drawable
        void createShaderProgram();

    protected:
        const std::string vertexShaderFileName;
        const std::string fragmentShaderFileName;
        unsigned shaderProgram{};
        unsigned vertexArrayObject{};
        unsigned elementBufferObject{};
        Camera::SharedCameraPointer camera;
        bool glyphsOn{};
        const unsigned supportedEffects{};
};

}