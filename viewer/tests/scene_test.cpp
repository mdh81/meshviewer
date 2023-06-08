#include "gtest/gtest.h"
#include "MockDrawable.h"
#include "../Viewport.h"
#include "../Scene.h"
#include "3dmath/Vector.h"
using namespace std;
using namespace mv::scene;

TEST(Scene, Creation) {
    Scene s1(10, 10);
    EXPECT_THROW({
        try {
            Scene s2(0, 10);
        } catch(exception& ex) {
            throw;
        }
    }, std::runtime_error) << "Expected an exception when scene's dimensions are zero";
}

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

TEST(Scene, AddViewports) {
    Scene scene(100, 100);
    scene.createViewport({{0.0f, 0.0f}, {0.5f,0.5f}});
    scene.createViewport({{0.5f, 0.5f}, {1.0f,1.0f}});
    ASSERT_EQ(scene.getViewports().size(), 3) << "Incorrect number of viewports added";
}

TEST(Scene, RemoveViewports) {
    Scene scene(100, 100);
    scene.createViewport({{0.0f, 0.0f}, {0.5f,0.5f}});
    scene.createViewport({{0.5f, 0.5f}, {1.0f,1.0f}});
    auto& viewports = scene.getViewports();
    auto hasViewport = [&scene](Viewport* viewport) {
        return std::find_if(scene.getViewports().begin(), scene.getViewports().end(),
                            [viewport](Scene::ViewportPointer const& viewportPtr) {
                                        return viewportPtr.get() == viewport;
                                    }) != scene.getViewports().end();
    };
    // viewports is a reference and as viewports are removed its size shrinks and for testing we remove viewports
    // at the head of the list hence the strange loop
    while(!viewports.empty()) {
        Viewport *viewportToRemove = viewports.at(0).get();
        scene.removeViewport(viewports.at(0));
        ASSERT_FALSE(hasViewport(viewportToRemove)) << "Viewport was not removed" << endl;
    }
    ASSERT_EQ(scene.getViewports().size(), 0) << "Expected all viewports to be removed";
}