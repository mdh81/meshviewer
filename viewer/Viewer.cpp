#include "Viewer.h"
#include "Types.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "Util.h"
#include "CameraFactory.h"
#include "EventHandler.h"
#include "CallbackFactory.h"
#include "Glyph.h"
#include "GradientBackground.h"
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
using namespace objects;

Viewer& Viewer::getInstance() {
    static Viewer instance;
    return instance;
}

Viewer::Viewer(unsigned windowWidth, unsigned windowHeight)
    : m_windowWidth(windowWidth)
    , m_windowHeight(windowHeight)
    , m_renderToImage(false)
    , m_frameBufferId(0)
    , m_imageTextureId(0)
    , m_showGradientBackground(true)
    , m_windowResized(false) {

    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    // Create GLFW window
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "MeshViewer", nullptr, nullptr);
    if(!m_window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW Window");
    }

    // Get notified when window size changes
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window,
                              [](GLFWwindow *window, int width, int height) {
          auto viewer = reinterpret_cast<Viewer*>(glfwGetWindowUserPointer(window));
          viewer->m_windowWidth = width;
          viewer->m_windowHeight = height;
          viewer->m_windowResized = true;
    });
    glfwMakeContextCurrent(m_window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Unable to initialize GLEW");
    }

    // Register event handlers
    EventHandler().registerCallback(
        Event(GLFW_KEY_W),
        CallbackFactory::getInstance().registerCallback
        (*this, &Viewer::setRenderMode, RenderMode::Wireframe));

    EventHandler().registerCallback(
        Event(GLFW_KEY_S),
        CallbackFactory::getInstance().registerCallback
        (*this, &Viewer::setRenderMode, RenderMode::Shaded));

    EventHandler().registerCallback(
            Event(GLFW_KEY_N),
            CallbackFactory::getInstance().registerCallback
            (*this, &Viewer::toggleNormalsDisplay));

    EventHandler().registerCallback(
            Event(GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT),
            CallbackFactory::getInstance().registerCallback
                    (*this, &Viewer::saveSnapshot));

    EventHandler().registerCallback(
            Event(GLFW_KEY_G),
            CallbackFactory::getInstance().registerCallback
                    (*this, &Viewer::toggleGradientBackgroundDisplay));

    // Start handling events
    EventHandler().start(m_window);
}

void Viewer::setColors() {
	// Dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

}

void Viewer::setRenderMode(const RenderMode rm) {
    if (rm == RenderMode::Wireframe) {
        m_outputStream << "Rendering mesh in wireframe" << endl;
    } else {
        m_outputStream << "Rendering mesh shaded" << endl;
    }
    m_renderMode = rm;
}

void Viewer::displayMesh(Mesh& mesh) {

    if (!m_window)
        throw std::runtime_error("Unexpected program state");

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Define colors
    setColors();

    // Create camera for the mesh
    Camera& camera = CameraFactory::getInstance().getCamera(mesh, {m_windowWidth, m_windowHeight});

    // Define normal glyphs
    Glyph gl = Glyph(mesh, common::GlyphAssociation::FaceNormal);

    // Create background geometry
    GradientBackground bg = GradientBackground();

    glEnable(GL_DEPTH_TEST);

    // Rendering loop
	do {

        if (m_windowResized) {
            camera.notifyWindowResized({m_windowWidth, m_windowHeight});
            m_windowResized = false;
        }

        if (m_renderToImage) {
            prepareOffscreenRender();
            glCallWithErrorCheck(glBindFramebuffer, GL_FRAMEBUFFER, m_frameBufferId);
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera
        camera.apply();

        // TODO: Replace with Scene::render()

        // Draw background
        if (m_showGradientBackground) {
            bg.render(camera);
        }

        // Draw mesh
        mesh.render(camera);

        // Draw glyph
        if (m_showNormals)
            gl.render(camera);

        // END TODO

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
    glCallWithErrorCheck(glViewport, 0, 0, m_windowWidth, m_windowHeight);

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
    auto status = stbi_write_jpg(imageFileName.string().data(), m_windowWidth, m_windowHeight, 3, rgbData.get(), 100);

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

}
