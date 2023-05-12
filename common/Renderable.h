#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "MeshViewerObject.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <string>
#include <unordered_set>
#include <vector>

namespace mv {

class Camera;

class Renderable : public MeshViewerObject {

    public:
        Renderable() = delete;
        Renderable(std::string const& vertexShaderFileName,
                   std::string const& fragmentShaderFileName);
        void createShaderProgram();
        virtual void render(Camera const&) = 0;

        // TODO: Support model coordinates for 3DRenderable objects
        // TODO: Use 3dmath matrix
        glm::mat4 getModelTransform() const { return glm::mat4(1.0); }

        void notifyWindowResized(unsigned windowWidth, unsigned windowHeight) {
            aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
            // TODO: This should trigger just projection re-computation not a geometry regenerate
            m_readyToRender = false;
        }

    protected:
        // Set the shader transform matrix inputs
        void setTransforms(Camera const &camera);

        // Generate data for the rendering pipeline
        virtual void generateRenderData() = 0;

        // Set colors for the rendered object
        virtual void generateColors() = 0;

    protected:
        const std::string m_vertexShaderFileName;
        const std::string m_fragmentShaderFileName;
        unsigned m_shaderProgram;
        unsigned m_vertexArrayObject;
        unsigned m_elementBufferObject;
        bool m_readyToRender;
        float aspectRatio;
};

using RenderableReference = std::reference_wrapper<Renderable const>;
using RenderableReferences = std::unordered_set<RenderableReference,
        MeshViewerObject::MeshViewerObjectHash,
        MeshViewerObject::MeshViewerObjectEquals>;
using Renderables = std::vector<RenderableReference>;

}

#endif
