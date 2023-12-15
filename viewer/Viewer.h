#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Drawable.h"
#include <memory>

class GLFWwindow;

namespace mv {

namespace scene {
    class Scene;
}

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
        [[nodiscard]] unsigned getWidth() const { return windowWidth; }
        [[nodiscard]] unsigned getHeight() const { return windowHeight; }
        [[nodiscard]] bool isDisplayingNormals() const { return showNormals; }
        enum class RenderMode {
            Wireframe,
            Shaded,
            ShadedWithEdges
        };
        [[nodiscard]] RenderMode getRenderMode() const { return renderMode; }
        [[nodiscard]] common::Point2D getCursorPosition() const { return cursorPosition; };
        [[nodiscard]] common::Point2D getCursorPositionDifference() const { return cursorPositionDifference; }
        [[nodiscard]] math3d::Matrix<float, 3, 3> getViewportToWindowTransform() const;

        // Creation Semantics
        static Viewer& getInstance();

    private:
        explicit Viewer(unsigned winWidth=1024, unsigned winHeight=768);
        class RenderLoop {
            public:
                RenderLoop() {}
                static void draw();
            private:
                static mv::Viewer* viewer;
                friend class mv::Viewer;
        };

        void createWindow();

#ifdef EMSCRIPTEN
        bool isCanvasResized(common::CanvasDimensions& canvasDimensions) const;
#endif

    public:
        Viewer(const Viewer&) = delete;
        Viewer(Viewer&&) = delete;
        Viewer& operator=(const Viewer&) = delete;
        Viewer& operator=(Viewer&&) = delete;

    // Member data
    private:
        unsigned windowWidth;
        unsigned windowHeight;
        unsigned frameBufferWidth;
        unsigned frameBufferHeight;
        GLFWwindow* window;
        bool showNormals;
        RenderMode renderMode;
        bool renderToImage;
        GLuint frameBufferId;
        GLuint imageTextureId;
        bool windowResized;
        Drawable::Drawables drawables;
        Drawable::Drawables activeObjects;
        common::Point2D cursorPosition;
        common::Point2D cursorPositionDifference;
        std::unique_ptr<mv::scene::Scene> scene;
        bool printGLInfoOnStartup;

    // Member functions
    private:
        static void setColors();

        void saveSnapshot() {
            renderToImage = true;
        }

        void prepareOffscreenRender();
        void saveAsImage();
};

}