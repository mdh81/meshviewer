#include "Viewer.h"
#include "Types.h"
#include "Scene.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "EventHandler.h"
#include "ConfigurationReader.h"

#ifdef OSX
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma clang diagnostic pop
#endif

#include "GLFW/glfw3.h"

#ifdef EMSCRIPTEN
#include "emscripten.h"
#include "emscripten/html5.h"
#endif

#include <filesystem>
#include <string>
using namespace std;

namespace mv {
using namespace common;
using namespace events;

Viewer* Viewer::RenderLoop::viewer = nullptr;

Viewer& Viewer::getInstance() {
    static Viewer instance;
    return instance;
}

Viewer::Viewer(unsigned windowWidth, unsigned windowHeight)
    : windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , frameBufferWidth(windowWidth)
    , frameBufferHeight(windowHeight)
    , renderToImage{}
    , frameBufferId(0)
    , imageTextureId(0)
    , windowResized{}
    , printGLInfoOnStartup{true} {

    // Register event handlers
    EventHandler eventHandler{};
    eventHandler.registerBasicEventCallback(
            Event{GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT}, *this, &Viewer::saveSnapshot);

    eventHandler.registerDataEventCallback(
            Event{events::EventId::FrameBufferResized}, *this, &Viewer::notifyFrameBufferResized);
    eventHandler.registerDataEventCallback(
            Event{events::EventId::CursorMoved}, *this, &Viewer::notifyCursorMoved);
    eventHandler.registerDataEventCallback(
            Event{events::EventId::Scrolled, GLFW_MOD_CONTROL}, *this, &Viewer::notifyMouseWheelOrTouchPadScrolled);
    eventHandler.registerDataEventCallback(
            Event{events::EventId::Scrolled, GLFW_MOD_SHIFT}, *this, &Viewer::notifyMouseWheelOrTouchPadScrolled);

    Viewer::RenderLoop::viewer = this;
}

void Viewer::createWindow() {

    if (window) {
        std::cerr << "Ignoring request to create window. Window was already created" << std::endl;
        return;
    }

    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    int width = static_cast<int>(windowWidth);
    int height = static_cast<int>(windowHeight);

#ifdef EMSCRIPTEN
    emscripten_get_canvas_element_size("#canvas", &width, &height);
    std::cerr << "Canvas element size = " << width << ", " << height << std::endl;
#endif

    // Create GLFW window
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifndef EMSCRIPTEN
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Mesh Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW Window");
    }
    glfwMakeContextCurrent(window);

#ifndef EMSCRIPTEN
    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Unable to initialize GLEW");
    }
#endif

    if (printGLInfoOnStartup) {
        auto glVersion = glCallWithErrorCheck(glGetString, GL_VERSION);
        auto glslVersion = glCallWithErrorCheck(glGetString, GL_SHADING_LANGUAGE_VERSION);
        std::cout << "OpenGL Version: " << glVersion << std::endl;
        std::cout << "GLSL Version:  "  << glslVersion << std::endl;
    }

    // Get initial framebuffer size
    glfwGetFramebufferSize(window, &width, &height);
    frameBufferWidth = static_cast<unsigned>(width);
    frameBufferHeight = static_cast<unsigned>(height);

    // Start handling events
    EventHandler().start(window);
}

void Viewer::notifyFrameBufferResized(events::EventData&& eventData) {
    if (eventData.size() != 2) {
        throw std::runtime_error("Incorrect event data for frame buffer resize event. Width and height "
                                 "of the resized frame buffer must be specified");
    }
    frameBufferWidth = std::any_cast<unsigned>(eventData.at(0));
    frameBufferHeight = std::any_cast<unsigned>(eventData.at(1));
    windowResized = true;
}

void Viewer::notifyCursorMoved(events::EventData&& eventData) {
    if (eventData.size() != 2) {
        throw std::runtime_error("Incorrect event data for frame buffer resize event. Width and height of the resized "
                                 "frame buffer must be specified");
    }
    common::Point2D currentCursorPosition {std::any_cast<float>(eventData[0]), std::any_cast<float>(eventData[1])};
    cursorPositionDifference = currentCursorPosition - cursorPosition;
    cursorPosition = currentCursorPosition;
}

void Viewer::notifyMouseWheelOrTouchPadScrolled(events::EventData&& eventData) {
    if (eventData.size() != 3) {
        throw std::runtime_error("Incorrect event data for scroll event. Scroll offsets and modifier keys are required "
                                 "to correctly process the scroll event");
    }
    cursorPositionDifference = {std::any_cast<float>(eventData[0]), std::any_cast<float>(eventData[1])};
    unsigned modifierKeys = std::any_cast<unsigned>(eventData[2]);
    EventHandler eventHandler;
    if (modifierKeys & GLFW_MOD_CONTROL) {
        eventHandler.raiseEvent(events::EventId::Zoomed, {cursorPosition, cursorPositionDifference});
    } else if (modifierKeys & GLFW_MOD_SHIFT) {
        eventHandler.raiseEvent(events::EventId::Panned, {cursorPosition, cursorPositionDifference});
    }
}

void Viewer::setColors() {
	// Dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
}

void Viewer::add(Drawable::Drawables const& newDrawables) {
    drawables.reserve(drawables.size() + newDrawables.size());
    for (auto& drawable : newDrawables) {
        drawables.push_back(drawable);
    }
}

#ifdef EMSCRIPTEN
bool Viewer::isCanvasResized(CanvasDimensions& canvasDimensions) const {
    double deviceWidth, deviceHeight;
    int canvasWidth, canvasHeight;
    emscripten_get_canvas_element_size("#canvas", &canvasWidth, &canvasHeight);
    emscripten_get_element_css_size("#canvas", &deviceWidth, &deviceHeight);
    double pixelRatio = emscripten_get_device_pixel_ratio();
    if (canvasWidth != static_cast<int>(deviceWidth * pixelRatio) ||
     canvasHeight != static_cast<int>(deviceHeight * pixelRatio)) {
        std::cout << "Emscripten Canvas Resized: "
                  << "Canvas width = " << canvasWidth << ' '
                  << "Canvas height = " << canvasHeight << ' '
                  << "CSS width = " << deviceWidth << ' '
                  << "CSS height = " << deviceHeight << ' '
                  << "Pixel ratio = " << pixelRatio << std::endl;
        canvasWidth = static_cast<int> (deviceWidth * pixelRatio);
        canvasHeight = static_cast<int> (deviceHeight * pixelRatio);
        emscripten_set_canvas_element_size("#canvas", canvasWidth, canvasHeight);
        canvasDimensions.width = canvasWidth;
        canvasDimensions.height = canvasHeight;
        return true;
    } else {
        return false;
    }
}
#endif

void Viewer::RenderLoop::draw() {
#ifndef EMSCRIPTEN
    do {
#endif

#ifdef EMSCRIPTEN
        CanvasDimensions canvasDimensions;
        if ((viewer->windowResized = viewer->isCanvasResized(canvasDimensions))) {
            glfwSetWindowSize(viewer->window, canvasDimensions.width, canvasDimensions.height);
        }
#endif

        if (viewer->windowResized) {
            viewer->scene->notifyWindowResized(viewer->frameBufferWidth, viewer->frameBufferHeight);
            viewer->windowResized = false;
        }

        if (viewer->renderToImage) {
            viewer->prepareOffscreenRender();
            glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, viewer->frameBufferId);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        viewer->scene->render();

        if (viewer->renderToImage) {
            viewer->saveAsImage();
        }

        // Swap buffers
        glfwSwapBuffers(viewer->window);
        glfwPollEvents();

#ifndef EMSCRIPTEN // Check if the ESC key was pressed or the window was closed
    }
    while (glfwGetKey(viewer->window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(viewer->window) == 0);
#endif
}

void Viewer::render() {

    if (!window)
        createWindow();

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Define colors
    setColors();

    // Create a scene
    DisplayDimensions displayDimensions{windowWidth, windowHeight, frameBufferWidth, frameBufferHeight};
    scene::Scene scene(displayDimensions);

    // Add renderables to the default scene
    for (auto& renderable : drawables) {
        scene->add(*renderable);
    }

    glEnable(GL_DEPTH_TEST);
    windowResized = true;

#ifndef EMSCRIPTEN
    RenderLoop().draw();
#else
    std::cerr << "Setting RenderLoop::draw() as emscripten main loop" << std::endl;
    emscripten_set_main_loop(RenderLoop::draw, 0, 1);
#endif
}

void Viewer::prepareOffscreenRender() {

    // Set viewport for the off-screen render
    glCallWithErrorCheck(glViewport, 0, 0, frameBufferWidth, frameBufferHeight);

    // Create frame buffer object
    glCallWithErrorCheck(glGenFramebuffers, 1, &frameBufferId);
    glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, frameBufferId);

    // Create color attachment point that binds to a texture
    glCallWithErrorCheck(glGenTextures, 1, &imageTextureId);
    glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, imageTextureId);
    glCallWithErrorCheck(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCallWithErrorCheck(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, imageTextureId, 0);
    glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, 0);

    // The rendering for the image should have depth test enabled. Since depth buffer won't be sampled, we can
    // use a render buffer
    GLuint renderBufferObject;
    glCallWithErrorCheck(glGenRenderbuffers, 1, &renderBufferObject);
    glCallWithErrorCheck(glBindRenderbuffer, GL_RENDERBUFFER, renderBufferObject);
#ifndef EMSCRIPTEN
    glCallWithErrorCheck(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
#else
    glCallWithErrorCheck(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, windowWidth, windowHeight);
#endif
    glCallWithErrorCheck(glBindRenderbuffer, GL_RENDERBUFFER, 0);
#ifndef EMSCRIPTEN
    glCallWithErrorCheck(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#else
    glCallWithErrorCheck(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
#endif

    // Check framebuffer status
    auto fboStatus = glCallWithErrorCheck(glCheckFramebufferStatus, GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
        cerr << "Framebuffer is incomplete" << endl;
        std::terminate();
    }

    // Unbind framebuffer so next framebuffer operations won't clobber this framebuffer
    glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, 0);

}

void Viewer::saveAsImage() {
#ifdef OSX
    // Create directory if it doesn't exist
    string outputDir = config::ConfigurationReader::getInstance().getValue("snapshotsDirectory");
    if (!filesystem::exists(outputDir)) {
        cerr << "Creating directory: " << outputDir << endl;
        filesystem::create_directory(outputDir);
    }
    // Find the image file name with the largest suffix and append 1 to it to arrive at
    // the name for the new snapshot
    auto snapshotPrefix = config::ConfigurationReader::getInstance().getValue("snapshotPrefix");
    unsigned long numericSuffix = 1;
    for (auto& dirEntry : filesystem::directory_iterator(outputDir)) {
        if (dirEntry.path().extension() == ".jpg") {
            auto baseFileName = dirEntry.path().stem().string();
            auto pos = baseFileName.find(snapshotPrefix);
            if (pos != string::npos) {
                numericSuffix = max(stoul(baseFileName.substr(pos + snapshotPrefix.size())) + 1, numericSuffix);
            }
        }
    }
    // Write snapshot to the file
    auto imageFileName = filesystem::path(outputDir) /
            filesystem::path(snapshotPrefix + to_string(numericSuffix) + ".jpg");

    // Read rgb pixels from the framebuffer
    unique_ptr<GLubyte[]> rgbData(new GLubyte[windowWidth * windowHeight * 3]);
    glCallWithErrorCheck(glReadPixels, 0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, rgbData.get());

    // Write to image. Flip the data because the viewport coordinate system's origin is on
    // bottom left for OpenGL. Most window systems have top-right corner as the origin so
    // the framebuffer data is flipped on the vertical axis when considered from the "normal"
    // windowing systems perspective
    stbi_flip_vertically_on_write(1);
    auto status = stbi_write_jpg(imageFileName.string().data(),
                                 static_cast<int>(windowWidth),
                                 static_cast<int>(windowHeight), 3, rgbData.get(), 100);

    // Unbind frame buffer, so the next draw call goes to the default frame buffer
    glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    // Delete frame buffer and reset state variables
    glCallWithErrorCheck(glDeleteFramebuffers, 1, &frameBufferId);
    frameBufferId = 0;
    renderToImage = false;
    // Reset viewport size to frame buffer size. This is important on osx with retina displays
    // where the resolution is twice the window size
    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    if (status) {
        cerr << "Wrote viewer contents to " << imageFileName.string() << endl;
    } else {
        cerr << "Failed to write to contents to image!" << endl;
    }
#endif
}

math3d::Matrix<float, 3, 3> Viewer::getViewportToWindowTransform() const {
    // Viewport coordinates are normalized, so they need to be scaled to window's size
    // Viewport and window have their x-coordinates pointing in the same direction, but
    // +Y goes up for viewport and down for window. We need to account for that via this
    // formula:
    // window.x = viewport.x * windowWidth + 0;
    // window.y = -viewport.y * windowHeight + windowHeight
    float windowWidth = static_cast<float>(this->windowWidth);
    float windowHeight = static_cast<float>(this->windowHeight);
    return math3d::Matrix<float, 3, 3> {
            {windowWidth,   0.f,            0.f},
            {0.f,           -windowHeight,  windowHeight},
            {0.f,            0.f,           1.f}
    };


}

}
