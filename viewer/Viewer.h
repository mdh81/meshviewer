#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "MeshViewerObject.h"
#include "Renderable.h"

#include "GL/glew.h"
#include <memory>

class GLFWwindow;

namespace mv {

// A viewer is a collection of scenes
class Viewer : public MeshViewerObject {
    public:
        ~Viewer() = default;
        // Add a renderable to the viewer and transfer ownership of it to the viewer
        void add(Renderable* renderable) { add(Renderable::RenderablePointer(renderable)); }
        void add(Renderable::RenderablePointer&& renderable) { renderables.push_back(std::move(renderable)); }
        // Add a list of renderables to the viewer and transfer ownership of them to the viewer
        void add(Renderable::Renderables& newRenderables);
        // Render scenes, viewports, and renderables
        void render();
        [[nodiscard]] unsigned getWidth() const { return m_windowWidth; }
        [[nodiscard]] unsigned getHeight() const { return m_windowHeight; }
        [[nodiscard]] bool isDisplayingNormals() const { return m_showNormals; }
        enum class RenderMode {
            Wireframe,
            Shaded,
            ShadedWithEdges
        };
        [[nodiscard]] RenderMode getRenderMode() const { return m_renderMode; }
        // Creation Semantics
        static Viewer& getInstance();

    private:
        explicit Viewer(unsigned winWidth=1024, unsigned winHeight=768);
    public:
        Viewer(const Viewer&) = delete;
        Viewer(Viewer&&) = delete;
        Viewer& operator=(const Viewer&) = delete;
        Viewer& operator=(Viewer&&) = delete;

    // Member data
    private:
        unsigned m_windowWidth;
        unsigned m_windowHeight;
        GLFWwindow* m_window;
        bool m_showNormals;
        RenderMode m_renderMode;
        bool m_renderToImage;
        GLuint m_frameBufferId;
        GLuint m_imageTextureId;
        bool m_windowResized;
        Renderable::Renderables renderables;
        RenderableReferences activeObjects;

    // Member functions
    private:
        static void setColors();

        void saveSnapshot() {
            m_renderToImage = true;
        }

        void prepareOffscreenRender();
        void saveAsImage();
};

}