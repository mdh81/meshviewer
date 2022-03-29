#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <exception>
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

    auto window = glfwCreateWindow(640, 480, "TestApp", nullptr, nullptr);
    if(!window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW Window");
    }
    glfwMakeContextCurrent(window);

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
    // Any call to glVertexAttribPointer will cause OpenGL to
    // store that information in this vao. This has the helpful
    // side-effect of not having to call glVertexAttribPointer
    // every time you switch to new vertex data 
    // TODO: Whatever the fuck the above means

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark grey background
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Define vertices
    // NOTE: Defined outside the render loop unlike the old-style GL
    // What makes this work is vbo
    float vertices[] = { 
        0.0, 0.5, 1.0, 0.0, 0.0, 
        -0.5, -0.5, 0.0, 1.0, 0.0,
        0.5, -0.5, 0.0, 0.0, 1.0 };

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
    // Again this is a bad design, hard-coding shader names into
    // c++ code is error-prone
    glBindFragDataLocation(shaderProgram, 0, "colorFS");

    // Link program
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Method 2: Set color by modifying "uniform"
    // Uniform variables are applied to each vertex
    // This is most likely what will be used for Mesh rendering
    // GLint colorId = glGetUniformLocation(shaderProgram, "triangleColor");
    // glUniform3f(colorId, 1.0, 0.2, 0.3);

    // Method 3: Set color via attributes 

    // Set Attributes
    // TODO: This is a shitty design, how the fuck does the c++ program
    // know what the attribute name in the shader is, it's awkward that
    // a change in the shader would need c++ code to be rebuilt 
    // EXPLORE ways to keep variable names in a config file and load
    // it into glsl and c++ code
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,            //attrib identifier
                          2,                    //number of values for this attribute
                          GL_FLOAT,             //data type 
                          GL_FALSE,             //data normalization status
                          5*sizeof(float),      //stride--each vertex has 5 float entries 
                          0                     //offset into the array
                         );
    
    GLint colorAttrib = glGetAttribLocation(shaderProgram, "colorVS");
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib,              //attrib identifier
                          3,                        //number of values for this attribute
                          GL_FLOAT,                 //data type 
                          GL_FALSE,                 //data normalization status
                          5*sizeof(float),          //stride--each vertex has 5 float entries 
                          (void*)(2*sizeof(float))  //offset into the array. 2 float entries are vertex
                                                    //coordinates
                         );


    // Rendering loop
	do {
        // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES,
                     0 /*skip count into vbo array*/,
                     3 /* Number of entries to consider from the vbo array*/);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while(!glfwWindowShouldClose(window));

    return 0;
}
