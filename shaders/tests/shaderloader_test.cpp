#include "gtest/gtest.h"
#include "ShaderLoader.h"
#include <fstream>

using namespace std;
using namespace mv;

class ShaderFixture : public testing::Test {

    protected:
        void SetUp() override {

            setenv("mv_UNIT_TESTING_IN_PROGRESS", "true", 1);

            // Create a subdirectory named shaders to match the structure the runtime expects
            std::filesystem::create_directory("./shaders");
            // Write vertex shader to file
            ofstream ofs("./shaders/vertex.shader");
            if (!ofs) throw std::runtime_error("Unable to open ./shaders/vertex.shader");
            ofs << "#version 410 core" << endl;
            ofs << "in vec2 position;" << endl;
            ofs << "void main() {" << endl;
            ofs << "gl_Position = vec4(position, 0.0, 1.0);" << endl;
            ofs << "}" << endl;
            ofs.close();
            
            // Write fragment shader to file
            ofs.open("./shaders/fragment.shader");
            if (!ofs) throw std::runtime_error("Unable to open ./shaders/fragment.shader");
            ofs << "#version 410 core" << endl;
            ofs << "out vec4 color;" << endl;
            ofs << "void main() {" << endl;
            ofs << "color = vec4(1.0, 1.0, 1.0, 1.0);" << endl;
            ofs << "}" << endl;
            ofs.close();
        }
};

TEST_F(ShaderFixture, TestLoadVertexShader) {
    string compilerOut;
    ASSERT_TRUE(get<0>(ShaderLoader().loadVertexShader("vertex.shader", compilerOut)));
}

TEST_F(ShaderFixture, TestLoadFragmentShader) {
    string compilerOut;
    ASSERT_TRUE(get<0>(ShaderLoader().loadFragmentShader("fragment.shader", compilerOut)));
}
