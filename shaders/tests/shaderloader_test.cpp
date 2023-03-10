#include "gtest/gtest.h"
#include "ShaderLoader.h"
#include <fstream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace std;
using namespace mv;

class ShaderFixture : public testing::Test {

    protected:
        void SetUp() override {
            // Write vertex shader to file
            ofstream ofs("./vertex.shader");
            if (!ofs) throw std::runtime_error("Unable to open ./vertex.shader");
            ofs << "#version 410 core" << endl;
            ofs << "in vec2 position;" << endl;
            ofs << "void main() {" << endl;
            ofs << "gl_Position = vec4(position, 0.0, 1.0);" << endl;
            ofs << "}" << endl;
            ofs.close();
            
            // Write fragment shader to file
            ofs.open("./fragment.shader");
            if (!ofs) throw std::runtime_error("Unable to open ./fragment.shader");
            ofs << "#version 410 core" << endl;
            ofs << "out vec4 color;" << endl;
            ofs << "void main() {" << endl;
            ofs << "color = vec4(1.0, 1.0, 1.0, 1.0);" << endl;
            ofs << "}" << endl;
            ofs.close();

            // Create a GL context so the GL calls below would succeed
            // Initialize GLFW
            if (!glfwInit())
                throw std::runtime_error("Failed to initialize GLFW");
            
            // Create GLFW window
            glfwWindowHint(GLFW_SAMPLES, 4); 
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            auto* window = glfwCreateWindow(100, 100, "TestWindow", nullptr, nullptr);
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
        }
};

TEST_F(ShaderFixture, TestLoadVertexShader) {
    string compilerOut;
    ASSERT_TRUE(get<0>(ShaderLoader().loadVertexShader("./vertex.shader", compilerOut)));
    ofstream ofs("vertexShader.compilerOut");
    ofs << compilerOut << endl;
    ofs.close();
}

TEST_F(ShaderFixture, TestLoadFragmentShader) {
    string compilerOut;
    ASSERT_TRUE(get<0>(ShaderLoader().loadFragmentShader("./fragment.shader", compilerOut)));
    ofstream ofs("fragmentShader.compilerOut");
    ofs << compilerOut << endl;
    ofs.close();
}
