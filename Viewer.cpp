#include "Viewer.h"
#include "Types.h"
#include "Mesh.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
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

void Viewer::displayMesh(const Mesh& mesh) {
    if (!m_window) 
        throw std::runtime_error("Unexpected program state");

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

    // Create vertex array object for managing vertex attributes
    // Currently we are not setting any vertex attributes
    // TODO: Write a primer GitHub article on VAO
    GLuint vaObj;
    glGenVertexArrays(1, &vaObj);
    glBindVertexArray(vaObj);

    // Create vertex buffer object
    // TODO: Write a primer GitHub article on VertexBufferObject 
    GLuint vbObj;
    glGenBuffers(1, &vbObj);
    // Make the vertex buffer object the current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbObj);

    // Upload vertex data to the vertex buffer object
    // TODO: Can I simply pass a pointer to vertices by casting struct Vertex* to
    // float*: So far it appears okay. Aligment for Vertex is 4 bytes, size is 12 bytes
    // a multiple of 4, so there is no padding in the Vertex object's memory layout
    /*size_t numBytes = 0;
    common::Vertex* vertexData;
    mesh.getVertexData(numBytes, &vertexData);
    glBufferData(GL_ARRAY_BUFFER, numBytes, reinterpret_cast<GLfloat*>(vertexData), GL_STATIC_DRAW);*/

    static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Rendering loop
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbObj);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, mesh.getNumberOfVertices());
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(m_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(m_window) == 0 );
}

}