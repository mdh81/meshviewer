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

    protected:
        // Set the shader transform matrix inputs
        void setModelViewProjection(Camera const&);
        void setModelView(Camera const&);
        
        // Generate data for the rendering pipeline
        virtual void generateRenderData() = 0;

        // Set colors for the rendered object
        virtual void generateColors() = 0;

    protected:
        const std::string m_vertexShaderFileName;
        const std::string m_fragmentShaderFileName;
        GLuint m_shaderProgram;
        GLuint m_vertexArrayObject;
        GLuint m_elementBufferObject;
        bool m_readyToRender;
};

using RenderableReference = std::reference_wrapper<Renderable const>;
using RenderableReferences = std::unordered_set<RenderableReference,
        MeshViewerObject::MeshViewerObjectHash,
        MeshViewerObject::MeshViewerObjectEquals>;
using Renderables = std::vector<RenderableReference>;

}

#endif
