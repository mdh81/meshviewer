#include "Viewer.h"
#include "Types.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "Util.h"
#include "CameraFactory.h"
#include "EventHandler.h"
#include "CallbackFactory.h"
#include "Glyph.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

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
    , m_windowHeight(windowHeight) {

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
    camera.debugOn();

    GLint shaderProg;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProg);
    cout << "Viewer shader program = " << shaderProg << endl;

    // Define normal glyphs
    Glyph gl = Glyph(mesh, common::GlyphAssociation::FaceNormal);

    glEnable(GL_DEPTH_TEST);

    // Rendering loop
	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera
        camera.apply();

        // TODO: Replace with Scene::render()

        // Draw mesh
        mesh.render(camera);

        // Draw glyph
        gl.render(camera);

        // END TODO

        // Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(m_window) == 0 );
}

}
