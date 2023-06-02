#include "Viewer.h"
#include "Types.h"
#include "Scene.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "EventHandler.h"
#include "CallbackFactory.h"
#include "ConfigurationReader.h"

#ifdef OSX
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma clang diagnostic pop
#endif

#include "GLFW/glfw3.h"

#include <filesystem>
#include <string>
using namespace std;

namespace mv {
using namespace common;
using namespace events;

Viewer& Viewer::getInstance() {
    static Viewer instance;
    return instance;
}

Viewer::Viewer(unsigned windowWidth, unsigned windowHeight)
    : m_windowWidth(windowWidth)
    , m_windowHeight(windowHeight)
    , m_frameBufferWidth(windowWidth)
    , m_frameBufferHeight(windowHeight)
    , m_renderToImage(false)
    , m_frameBufferId(0)
    , m_imageTextureId(0)
    , m_windowResized(false)
    , m_renderMode(RenderMode::Shaded)
    , m_showNormals(false) {

    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    // Create GLFW window
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(static_cast<int>(m_windowWidth),
                                static_cast<int>(m_windowHeight),
                                "MeshViewer", nullptr, nullptr);
    if(!m_window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW Window");
    }

    // Get initial framebuffer size
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    m_frameBufferWidth = static_cast<unsigned>(width);
    m_frameBufferHeight = static_cast<unsigned>(height);

    // Get notified when window size changes
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window,
                              [](GLFWwindow *window, int width, int height) {
          auto viewer = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
          viewer->m_frameBufferWidth = width;
          viewer->m_frameBufferHeight = height;
          viewer->m_windowResized = true;
    });
    glfwMakeContextCurrent(m_window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Unable to initialize GLEW");
    }

    // Keep track of cursor position to handle various interaction gestures
    glfwSetCursorPosCallback(m_window,[](GLFWwindow* window, double x, double y) {
        auto viewer = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
        common::Point2D currentCursorPosition {static_cast<float>(x), static_cast<float>(y)};
        viewer->cursorPositionDifference = currentCursorPosition - viewer->cursorPosition;
        viewer->cursorPosition = currentCursorPosition;
    });
    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
        auto viewer = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
        viewer->cursorPositionDifference = {static_cast<float>(xOffset), static_cast<float>(yOffset)};
        unsigned modifierKeys = 0;
        if (EventHandler().isModifierKeyPressed(GLFW_MOD_SHIFT)) {
            modifierKeys |= GLFW_MOD_SHIFT;
        }
        if (EventHandler().isModifierKeyPressed(GLFW_MOD_CONTROL)) {
            modifierKeys |= GLFW_MOD_CONTROL;
        }
        EventHandler().raiseEvent(Event(MOUSE_WHEEL_EVENT, modifierKeys));
    });
    // Register event handlers
    EventHandler().registerCallback(
            Event(GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT),
            CallbackFactory::getInstance().registerCallback
                    (*this, &Viewer::saveSnapshot));

    // Start handling events
    EventHandler().start(m_window);
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

void Viewer::render() {

    if (!m_window)
        throw std::runtime_error("Unexpected program state");

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Define colors
    setColors();

    // Create a scene
    scene::Scene scene(m_frameBufferWidth, m_frameBufferHeight);

    // Add renderables to the default scene
    for (auto& renderable : drawables) {
        scene.add(*renderable);
    }

    glEnable(GL_DEPTH_TEST);
    m_windowResized = true;
    // Rendering loop
	do {

        if (m_windowResized) {
            scene.notifyWindowResized(m_frameBufferWidth, m_frameBufferHeight);
            m_windowResized = false;
        }

        if (m_renderToImage) {
            prepareOffscreenRender();
            glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, m_frameBufferId);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.render();

        if (m_renderToImage) {
            saveAsImage();
        }

        // Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(m_window) == 0 );
}

void Viewer::prepareOffscreenRender() {

    // Set viewport for the off-screen render
    glCallWithErrorCheck(glViewport, 0, 0, m_frameBufferWidth, m_frameBufferHeight);

    // Create frame buffer object
    glCallWithErrorCheck(glGenFramebuffers, 1, &m_frameBufferId);
    glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, m_frameBufferId);

    // Create color attachment point that binds to a texture
    glCallWithErrorCheck(glGenTextures, 1, &m_imageTextureId);
    glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, m_imageTextureId);
    glCallWithErrorCheck(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, m_windowWidth, m_windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCallWithErrorCheck(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_imageTextureId, 0);
    glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, 0);

    // The rendering for the image should have depth test enabled. Since depth buffer won't be sampled, we can
    // use a render buffer
    GLuint renderBufferObject;
    glCallWithErrorCheck(glGenRenderbuffers, 1, &renderBufferObject);
    glCallWithErrorCheck(glBindRenderbuffer, GL_RENDERBUFFER, renderBufferObject);
    glCallWithErrorCheck(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_windowWidth, m_windowHeight);
    glCallWithErrorCheck(glBindRenderbuffer, GL_RENDERBUFFER, 0);
    glCallWithErrorCheck(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);

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
    unique_ptr<GLubyte[]> rgbData(new GLubyte[m_windowWidth * m_windowHeight * 3]);
    glCallWithErrorCheck(glReadPixels, 0, 0, m_windowWidth, m_windowHeight, GL_RGB, GL_UNSIGNED_BYTE, rgbData.get());

    // Write to image. Flip the data because the viewport coordinate system's origin is on
    // bottom left for OpenGL. Most window systems have top-right corner as the origin so
    // the framebuffer data is flipped on the vertical axis when considered from the "normal"
    // windowing systems perspective
    stbi_flip_vertically_on_write(1);
    auto status = stbi_write_jpg(imageFileName.string().data(),
                                 static_cast<int>(m_windowWidth),
                                 static_cast<int>(m_windowHeight), 3, rgbData.get(), 100);

    // Unbind frame buffer, so the next draw call goes to the default frame buffer
    glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, 0);

    // Delete frame buffer and reset state variables
    glCallWithErrorCheck(glDeleteFramebuffers, 1, &m_frameBufferId);
    m_frameBufferId = 0;
    m_renderToImage = false;
    // Reset viewport size to frame buffer size. This is important on osx with retina displays
    // where the resolution is twice the window size
    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(m_window, &frameBufferWidth, &frameBufferHeight);
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    if (status) {
        cerr << "Wrote viewer contents to " << imageFileName.string() << endl;
    } else {
        cerr << "Failed to write to contents to image!" << endl;
    }

}

math3d::Matrix<float, 3, 3> Viewer::getViewportToWindowTransform() const {
    // Viewport coordinates are normalized, so they need to be scaled to window's size
    // Viewport and window have their x-coordinates pointing in the same direction, but
    // +Y goes up for viewport and down for window. We need to account for that via this
    // formula:
    // window.x = viewport.x * windowWidth + 0;
    // window.y = -viewport.y * windowHeight + windowHeight
    float windowWidth = static_cast<float>(m_windowWidth);
    float windowHeight = static_cast<float>(m_windowHeight);
    return math3d::Matrix<float, 3, 3> {
            {windowWidth,   0.f,            0.f},
            {0.f,           -windowHeight,  windowHeight},
            {0.f,            0.f,           1.f}
    }.transpose();


}

}
