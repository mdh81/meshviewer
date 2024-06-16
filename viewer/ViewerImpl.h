#pragma once
#include "Viewer.h"
#include "EventTypes.h"
#include "Scene.h"
#include <memory>

class GLFWwindow;

namespace mv::viewer {
    class ViewerImpl : public Viewer, public MeshViewerObject {
    public:
        void add(Drawable::DrawablePointer&& drawable) override;
        void add(Drawable::DrawablePointers const& newDrawables) override;
        void add(Drawable::DrawablePointer const&) override;
        void remove(Drawable::DrawablePointer const&) override;
        void render() override;
        [[nodiscard]] math3d::Matrix<float, 3, 3> getViewportToWindowTransform() const override;

        // TODO: Make more of these methods interfaces as more unit tests are written and mock viewer is used
        // for testing
        [[nodiscard]] unsigned getWidth() const { return windowWidth; }
        [[nodiscard]] unsigned getHeight() const { return windowHeight; }
        [[nodiscard]] common::Point2D getCursorPosition() const { return cursorPosition; };
        [[nodiscard]] common::Point2D getCursorPositionDifference() const { return cursorPositionDifference; }
        void notifyFrameBufferResized(events::EventData&&);
        void notifyCursorMoved(events::EventData&&);
        void notifyMouseWheelOrTouchPadScrolled(events::EventData&&);
        void notifyLeftMousePressed();
        void notifyLeftMouseReleased();


    private:
        class RenderLoop {
        public:
            RenderLoop() {}
            static void draw();
        private:
            static mv::viewer::ViewerImpl* viewer;
            friend class mv::viewer::ViewerImpl;
        };

        void createWindow();

#ifdef EMSCRIPTEN
        bool isCanvasResized(common::CanvasDimensions& canvasDimensions) const;
#endif

    // Construction semantics. No copies or moves supported. Private constructor and destructor only available to
    // the friend factory class
    private:
        friend class ViewerFactory;
        explicit ViewerImpl(unsigned winWidth=1024, unsigned winHeight=768);
        ~ViewerImpl() = default;
        ViewerImpl(const Viewer&) = delete;
        ViewerImpl(Viewer&&) = delete;
        ViewerImpl& operator=(const Viewer&) = delete;
        ViewerImpl& operator=(Viewer&&) = delete;

        // Member data
    private:
        unsigned windowWidth;
        unsigned windowHeight;
        unsigned frameBufferWidth;
        unsigned frameBufferHeight;
        GLFWwindow* window;
        bool renderToImage;
        GLuint frameBufferId;
        GLuint imageTextureId;
        bool windowResized;
        Drawable::DrawablePointers drawables;
        Drawable::DrawablePointers activeObjects;
        common::Point2D cursorPosition;
        common::Point2D cursorPositionDifference;
        common::UniquePointer<mv::scene::Scene> scene;
        bool printGLInfoOnStartup;
        bool leftMouseDown{};

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