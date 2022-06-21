#include "Viewer.h"
#include "Types.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "Util.h"
#include "CameraFactory.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

#include <string>
using namespace std;

namespace meshviewer {
using namespace common;

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
}

// TODO: Designate a directory to load shaders from
GLuint Viewer::createShaderProgram() {
    
    // Vertex Shader
    string compilerOut;
    auto status = meshviewer::ShaderLoader().loadVertexShader("./shaders/vertex.shader", compilerOut);
    if (!get<0>(status)) {
        throw compilerOut;
    }
    GLuint vertexShaderId = get<1>(status);

    // Fragment Shader
    status = meshviewer::ShaderLoader().loadFragmentShader("./shaders/fragment.shader", compilerOut);
    if (!get<0>(status)) {
        throw compilerOut;
    }
    GLuint fragmentShaderId = get<1>(status);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId); 
    glBindFragDataLocation(shaderProgram, 0, "colorFS");

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
		cerr << pProgramErrorMessage.get();
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

    // Create vertex buffer object
    GLuint vbObj;
    glGenBuffers(1, &vbObj);
    // Make the vertex buffer object the current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbObj);

    // Upload vertex data to the vertex buffer object
    // Casting Vertex* to float* seems okay since the struct Vertex doesn't have
    // any padding due to the fact that it's alignment being 4 bytes and it's total
    // size being 12 (a multiple of 4).
    size_t numBytes = 0;
    common::Vertex* vertexData;
    mesh.getVertexData(numBytes, vertexData);
    glBufferData(GL_ARRAY_BUFFER, numBytes, reinterpret_cast<GLfloat*>(vertexData), GL_STATIC_DRAW);

    // Define layout of vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,            //attrib identifier
                          3,                    //number of values for this attribute
                          GL_FLOAT,             //data type 
                          GL_FALSE,             //data normalization status
                          3*sizeof(float),      //stride--each vertex has 3 float entries 
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

void Viewer::setColors(const GLuint shaderProgram) {

	// Dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    
    // Set wireframe color for the mesh 
    GLint colorId = glGetUniformLocation(shaderProgram, "wireframeColor");
    glUniform3fv(colorId, 1, glm::value_ptr(glm::vec3(0.76f, 0.61f, 0.2f)));
}

void Viewer::setView(const Mesh& mesh, const GLuint shaderProgram) {
    Camera& camera = CameraFactory::getInstance().getCamera(mesh, CameraFactory::ProjectionType::Perspective); 
    camera.setOrbitOn(common::Axis::Y);

//
//    // Set up transform matrices
//    GLuint matrixId = glGetUniformLocation(shaderProgram, "transformMatrix");
//
//    // Projection: Converts to homogenous coordinates
//    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.f), 640.f/480.f, 0.1f, 100.0f);
//
//    // View: Converts to camera coordinates
//    glm::mat4 viewMatrix = glm::mat4(1.0f);
//    viewMatrix[3] = glm::vec4(0,0,-20,1);
//    
//    // Model: Converts to world coordinates. 
//    // This demo model is in the world coordinates, so we have an identity 
//    glm::mat4 modelMatrix = glm::mat4(1.0f);
//
//    // Combined Transform
//    glm::mat4 compositeTransform = projectionMatrix * viewMatrix * modelMatrix;
//
//    glUniformMatrix4fv(matrixId,
//                       1 /*num matrices*/,
//                       GL_FALSE /*transpose*/,
//                       &compositeTransform[0][0]);

}

void Viewer::displayMesh(const Mesh& mesh) {
    // Window must have been created by the time display is called
    if (!m_window) 
        throw std::runtime_error("Unexpected program state");

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Load shaders
    GLuint shaderProgram = createShaderProgram(); 

    // Define vertices
    setVertexData(mesh, shaderProgram);

   // Define elements
   setElementData(mesh); 

    // Define colors
    setColors(shaderProgram);
    
    // Setup model, view and projection transformations
    setView(mesh, shaderProgram);

    Camera& camera = CameraFactory::getInstance().getCamera(mesh, CameraFactory::ProjectionType::Orthographic);
    camera.debugOn();

    // Rendering loop
	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

        // Update camera
        camera.apply(shaderProgram);

		// Draw
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES,
                       mesh.getNumberOfVertices(), // Number of elements
                       GL_UNSIGNED_INT,            // Type of element buffer data
                       0                           // Offset into element buffer data       
                      );
		
        // Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(m_window) == 0 );
}

}
