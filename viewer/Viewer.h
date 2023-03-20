#ifndef MESH_VIEWER_VIEWER_H
#define MESH_VIEWER_VIEWER_H

#include "Mesh.h"
#include "Camera.h"
#include "MeshViewerObject.h"
#include "Renderable.h"

#include "GL/glew.h"
#include <memory>

class GLFWwindow;

namespace mv {

// At the moment there is a 1-1 relationship between a Viewer and a mesh. In the
// future, when we support multiple mesh objects the relationship will be 1-N
class Viewer : public MeshViewerObject {
    public:
        ~Viewer() = default;
        void displayMesh(Mesh& mesh);
        unsigned getWidth() const { return m_windowWidth; }
        unsigned getHeight() const { return m_windowHeight; }
        bool isDisplayingNormals() const { return m_showNormals; }
        enum class RenderMode {
            Wireframe,
            Shaded,
        };
        RenderMode getRenderMode() const { return m_renderMode; }
        // Creation Semantics
        static Viewer& getInstance();

    private:
        Viewer(unsigned winWidth=1024, unsigned winHeight=768);
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

    // Member functions
    private:
        void setColors();
        void setRenderMode(const RenderMode);
        void toggleNormalsDisplay() {
            m_showNormals = !m_showNormals;
        }
        void saveSnapshot() {
            m_renderToImage = true;
        }
        void prepareOffscreenRender();
        void saveAsImage();
};

}

#endif
