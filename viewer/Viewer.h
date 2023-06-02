#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "MeshViewerObject.h"
#include "Drawable.h"

#include "GL/glew.h"
#include <memory>

class GLFWwindow;

namespace mv {

// A viewer is a collection of scenes
class Viewer : public MeshViewerObject {
    public:
        ~Viewer() = default;
        // Add a drawable to the viewer and transfer ownership of it to the viewer
        void add(Drawable* drawable) { add(Drawable::DrawablePointer(drawable)); }
        void add(Drawable::DrawablePointer&& drawable) { drawables.push_back(std::move(drawable)); }
        // Add a list of drawables to the viewer and transfer ownership of them to the viewer
        void add(Drawable::Drawables const& newDrawables);
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
        [[nodiscard]] common::Point2D getCursorPosition() const { return cursorPosition; };
        [[nodiscard]] common::Point2D getCursorPositionDifference() const { return cursorPositionDifference; }
        [[nodiscard]] math3d::Matrix<float, 3, 3> getViewportToWindowTransform() const;

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
        unsigned m_frameBufferWidth;
        unsigned m_frameBufferHeight;
        GLFWwindow* m_window;
        bool m_showNormals;
        RenderMode m_renderMode;
        bool m_renderToImage;
        GLuint m_frameBufferId;
        GLuint m_imageTextureId;
        bool m_windowResized;
        Drawable::Drawables drawables;
        Drawable::Drawables activeObjects;
        common::Point2D cursorPosition;
        common::Point2D cursorPositionDifference;

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