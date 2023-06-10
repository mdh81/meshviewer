#include "gtest/gtest.h"
#include "Viewer.h"
#include "EventHandler.h"
#include <memory>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::events;

namespace mv::viewer {
    class ViewerTest {
    public:
        static void simulateKeyPress(int key) { EventHandler::handleKeyPress(nullptr, key, 0, GLFW_PRESS, 0); }
    };
}

TEST(Viewer, Singleton) {
    Viewer& v = Viewer::getInstance();
    Viewer& v1 = Viewer::getInstance();
    ASSERT_EQ(&v, &v1) << "Viewer::getInstance() returned more than one singleton instance";
}

TEST(Viewer, Getters) {
    Viewer& v = Viewer::getInstance();
    ASSERT_EQ(v.isDisplayingNormals(), false) << "Normals add is supposed to be off by default";
    mv::viewer::ViewerTest::simulateKeyPress(GLFW_KEY_N);
    ASSERT_EQ(v.isDisplayingNormals(), true) << "Normals add is not true";
    mv::viewer::ViewerTest::simulateKeyPress(GLFW_KEY_W);
    ASSERT_EQ(v.getRenderMode(), mv::Viewer::RenderMode::Wireframe) << "Render mode is not wireframe";
    mv::viewer::ViewerTest::simulateKeyPress(GLFW_KEY_S);
    ASSERT_EQ(v.getRenderMode(), mv::Viewer::RenderMode::Shaded) << "Render mode is not shaded";
}

TEST(Viewer, ConvertViewportToWindowCoordinates) {
    Viewer& v = Viewer::getInstance(/*width=1024, height=768*/);
    auto viewportToWindow = v.getViewportToWindowTransform();
    auto windowCoordinates = viewportToWindow * common::Point3D{1, 1, 1};
    ASSERT_FLOAT_EQ(windowCoordinates.x, 1024.f) << "Wrong window coordinates for viewport coordinate 1,1";
    ASSERT_FLOAT_EQ(windowCoordinates.y, 0.f) << "Wrong window coordinates for viewport coordinate 1,1";

    windowCoordinates = viewportToWindow * common::Point3D{1, 0, 1};
    ASSERT_FLOAT_EQ(windowCoordinates.x, 1024.f) << "Wrong window coordinates for viewport coordinate 1,0";
    ASSERT_FLOAT_EQ(windowCoordinates.y, 768.f) << "Wrong window coordinates for viewport coordinate 1,0";

    windowCoordinates = viewportToWindow * common::Point3D{0, 0, 1};
    ASSERT_FLOAT_EQ(windowCoordinates.x, 0.f) << "Wrong window coordinates for viewport coordinate 0,0";
    ASSERT_FLOAT_EQ(windowCoordinates.y, 768.f) << "Wrong window coordinates for viewport coordinate 0,0";

    windowCoordinates = viewportToWindow * common::Point3D{0, 1, 1};
    ASSERT_FLOAT_EQ(windowCoordinates.x, 0.f) << "Wrong window coordinates for viewport coordinate 0,1";
    ASSERT_FLOAT_EQ(windowCoordinates.y, 0.f) << "Wrong window coordinates for viewport coordinate 0,1";
}
