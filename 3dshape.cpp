#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <exception>
#include <array>
#include <memory>
#include <chrono>
#include <cmath>
using namespace std;

#include "ShaderLoader.h"

int main() {
    
    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    
    // Create GLFW window
    glfwWindowHint(GLFW_SAMPLES, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(640, 480, "3DShape", nullptr, nullptr);
    if(!window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW Window");
    }
    glfwMakeContextCurrent(window);
    
    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


    // Initialize GLEW 
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Unable to initialize GLEW");
    }

    // Vertex Array Object
    // Used to manage vertex attributes
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	// Dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Vertices of a tetrahedron
    float tetVertices[] = { 
        -1.0,  -1.0,  1.0,
         1.0,  -1.0,  1.0, 
         0.0,  -1.0, -1.0, 
         0.0,   1.0,  0.0, 
    };

    // Create VBO and transfer the data to graphics card memory
    GLuint vbo;
    glGenBuffers(1 /*generate 1 buffer*/, &vbo);

    // Make VBO current by making it the current array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Set data to the current array buffer, which is our VBO
    // The last argument says that the vertices are uploaded to the
    // graphics card once and redrawn multiple times. This forces the
    // graphics card to store it in the most efficient memory bank that
    // allows fast reads
    glBufferData(GL_ARRAY_BUFFER, sizeof(tetVertices), tetVertices, GL_STATIC_DRAW);

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
		cout << pProgramErrorMessage.get();
	}

    // TODO: Why does the tutorial delete detach and delete shader?

    // Set a single color for the tet
    // Uniform variables are applied to each vertex
    GLint colorId = glGetUniformLocation(shaderProgram, "wireframeColor");
    glUniform3f(colorId, 0.7, 0.2, 0.2);

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
    
    // Define connectivity array
    GLuint faces[] = {0, 2, 1, 
                      0, 3, 2,
                      1, 2, 3, 
                      0, 1, 3};
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

    // Set up transform matrices
    GLuint matrixId = glGetUniformLocation(shaderProgram, "transformMatrix");

    // Projection: Converts to homogenous coordinates
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.f), 640.f/480.f, 0.1f, 100.0f);

    // View: Converts to camera coordinates
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    viewMatrix[3] = glm::vec4(0,0,-5,1);
    
    // Model: Converts to world coordinates. 
    // This demo model is in the world coordinates, so we have an identity 
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    float rotY = 0;

    auto start = chrono::steady_clock::now();

    // Rendering loop
	do {
        // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(now-start).count();

        if (elapsed >= 100) {
            rotY += 5.f;
            start = chrono::steady_clock::now();
        }

        // Rotate about y-axis by rotY
        viewMatrix[0][0] = cos(rotY * 0.0174533);
        viewMatrix[0][1] = 0.0f;
        viewMatrix[0][2] = -sin(rotY * 0.0174533);
        
        viewMatrix[1][0] = 0.0f;
        viewMatrix[1][1] = 1.0f;
        viewMatrix[1][2] = 0.0f; 
        
        viewMatrix[2][0] = -sin(rotY * 0.0174533);
        viewMatrix[2][1] = 0.0f;
        viewMatrix[2][2] = -cos(rotY * 0.0174533);
        
        // Combined Transform
        glm::mat4 compositeTransform = projectionMatrix * viewMatrix * modelMatrix;

        glUniformMatrix4fv(matrixId,
                           1 /*num matrices*/,
                           GL_FALSE /*transpose*/,
                           &compositeTransform[0][0]);

        // Draw Elements
        glDrawElements(GL_LINES,
                       12,              // Number of indices in the connectivity array
                       GL_UNSIGNED_INT, // Type of the array
                       0                // Offset into the array
                      );
        
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while(!glfwWindowShouldClose(window));

    return 0;
}
