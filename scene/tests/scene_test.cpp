#include "gtest/gtest.h"
#include "MockRendererable.h"
#include "Viewport.h"
#include "Scene.h"
#include "3dmath/Vector.h"
using namespace std;
using namespace mv::scene;

TEST(Scene, CreationOfDefaultViewport) {
    Scene scene(100, 100);
    auto& viewports = scene.getViewports();
    auto numViewports = viewports.size();
    ASSERT_EQ(numViewports, 1) << "Default viewport not created";
    ASSERT_FLOAT_EQ(viewports.at(0)->getOrigin().x, 0.0f) << "Default viewport origin is incorrect";
    ASSERT_FLOAT_EQ(viewports.at(0)->getOrigin().y, 0.0f) << "Default viewport origin is incorrect";
    ASSERT_FLOAT_EQ(viewports.at(0)->getWidth(), 1.0f) << "Default viewport width is incorrect";
    ASSERT_FLOAT_EQ(viewports.at(0)->getHeight(), 1.0f) << "Default viewport height is incorrect";
}