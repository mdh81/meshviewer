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

namespace meshviewer {
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

// TODO: Designate a directory to load shaders from
GLuint Viewer::createShaderProgram() {

    // Vertex Shader
    string compilerOut;
    auto status = meshviewer::ShaderLoader().loadVertexShader("./shaders/MeshVertex.shader", compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint vertexShaderId = get<1>(status);

    // Fragment Shader
    status = meshviewer::ShaderLoader().loadFragmentShader("./shaders/Fragment.shader", compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint fragmentShaderId = get<1>(status);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
    glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

    // Link program
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Check the program
    GLint result;
    int infoLogLength;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::unique_ptr<char> pProgramErrorMessage {new char[infoLogLength+1]};
		glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, pProgramErrorMessage.get());
		m_outputStream << pProgramErrorMessage.get();
        throw std::runtime_error("Failed to load shaders" + std::string(pProgramErrorMessage.get()));
	}

    return shaderProgram;
}

void Viewer::setVertexData(const Mesh& mesh, const GLuint shaderProgram) {
    // Create vertex array object for managing vertex attributes
    // Currently we are not setting any vertex attributes
    GLuint vaObj;
    glGenVertexArrays(1, &vaObj);
    glBindVertexArray(vaObj);

    // Create vertex buffer object for storing vertex data
    GLuint vbObjVerts;
    glGenBuffers(1, &vbObjVerts);
    // Make the vertex buffer object the current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbObjVerts);

    // Upload vertex data to the vertex buffer object
    auto const vertexData = mesh.getVertexData();
    glBufferData(GL_ARRAY_BUFFER, vertexData.getDataSize(), vertexData.getData(), GL_STATIC_DRAW);

    // Define layout of vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vertexPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,            //attrib identifier
                          3,                    //number of values for this attribute
                          GL_FLOAT,             //data type
                          GL_FALSE,             //data normalization status
                          3*sizeof(float),      //stride--each vertex has 3 float entries
                          0                     //offset into the array
                         );

    // Create VBO for normals
    GLuint vbObjNormals;
    glGenBuffers(1, &vbObjNormals);

    // Make the normals vertex buffer object the current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbObjNormals);

    // Upload normals to the vertex buffer object
    auto const normalData = mesh.getNormals(common::NormalLocation::Vertex);
    glBufferData(GL_ARRAY_BUFFER, normalData.getDataSize(), normalData.getData(), GL_STATIC_DRAW);

    // Define layout of normal data
    GLint normalAttrib = glGetAttribLocation(shaderProgram, "vertexNormal");
    glEnableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(normalAttrib,         //attrib identifier
                          3,                    //number of values for this attribute
                          GL_FLOAT,             //data type
                          GL_FALSE,             //data normalization status
                          3*sizeof(float),      //stride--each normal has 3 float entries
                          0                     //offset into the array
                         );
}

void Viewer::setElementData(const Mesh& mesh) {

    // Create element buffer object
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Upload connectivity data to element buffer object
    size_t numBytes;
    GLuint* faceData;
    mesh.getConnectivityData(numBytes, faceData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, faceData, GL_STATIC_DRAW);

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
    // Window must have been created by the time display is called
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
    //Glyph gl = Glyph(mesh, common::GlyphAssociation::FaceNormal);
    //gl.build();

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
        //gl.render(camera);

        // END TODO

        // Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(m_window) == 0 );
}

}
