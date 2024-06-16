#include "gtest/gtest.h"
#include "Mock3DDrawable.h"
#include "../Viewport.h"
#include "3dmath/Vector.h"
#include "ViewerFactory.h"
#include "../common/Environment.h"
#include "Util.h"
using namespace std;
using namespace mv::scene;
namespace mv::scene {
    class ViewportTest : public ::testing::Test {
    public:
        bool isViewportEvent(Viewport const &viewport, mv::common::Point2D const &cursorPosition) {
            return viewport.isViewportEvent(cursorPosition);
        }

        void SetUp() override {
            setenv("mv_UNIT_TESTING_IN_PROGRESS", "true", 1);
        }

    };

    TEST_F(ViewportTest, Creation) {
        Viewport viewport({{0.0f, 0.0f}, {1.0f, 1.0f}});
        ASSERT_FLOAT_EQ(viewport.getOrigin().x, 0.0f) << "Origin x-coordinate is incorrect";
        ASSERT_FLOAT_EQ(viewport.getOrigin().y, 0.0f) << "Origin y-coordinate is incorrect";
        ASSERT_FLOAT_EQ(viewport.getWidth(), 1.0f) << "Width is incorrect";
        ASSERT_FLOAT_EQ(viewport.getHeight(), 1.0f) << "Height is incorrect";
    }

    TEST_F(ViewportTest, DrawablesManagement) {
        Viewport viewport({{0.0f, 0.0f}, {1.0f, 1.0f}});
        mv::Mock3DDrawable drawable1, drawable2;
        viewport.add(drawable1);
        viewport.add(drawable2);
        ASSERT_EQ(viewport.getDrawables().size(), 2) << "Unexpected number of renderables after add";
        ASSERT_TRUE(viewport.getDrawables().at(0).get().getId() == drawable1.getId() ||
                    viewport.getDrawables().at(0).get().getId() == drawable2.getId())
                                    << "Unexpected renderable after add";
        ASSERT_TRUE(viewport.getDrawables().at(1).get().getId() == drawable1.getId() ||
                    viewport.getDrawables().at(1).get().getId() == drawable2.getId())
                                    << "Unexpected renderable after add";
        viewport.remove(drawable2);
        ASSERT_EQ(viewport.getDrawables().size(), 1) << "Unexpected number of renderables after remove";
        ASSERT_TRUE(viewport.getDrawables().at(0).get().getId() == drawable1.getId())
                                    << "Unexpected renderable after remove";
        ASSERT_THROW({
                         try {
                             auto drawable = mv::Mock3DDrawable();
                             viewport.remove(drawable);
                         } catch (std::runtime_error &) {
                             throw;
                         }
                     }, std::runtime_error)
                                    << "Expected an exception to be thrown when a non-existent drawable is removed";
    }

    TEST_F(ViewportTest, IsViewportEvent) {
        auto& v = viewer::ViewerFactory{}.getViewer();
        Viewport viewport({{0.0f, 0.0f}, {1.0f, 1.0f}});
        viewport.notifyDisplayResized({1024, 768, 1024, 768});
        ASSERT_TRUE(isViewportEvent(viewport, common::Point2D {1024.f/2.f, 768.f/2.f}))
                                    << "Cursor position at window center wrongly classified as outside the viewport";
        ASSERT_TRUE(isViewportEvent(viewport, common::Point2D {0.f, 0.f}))
                                    << "Cursor position at window origin wrongly classified as outside the viewport";

        ASSERT_TRUE(isViewportEvent(viewport, common::Point2D {1024.f, 768.f}))
                                    << "Cursor position at window max is wrongly classified as outside the viewport";

        ASSERT_FALSE(isViewportEvent(viewport, common::Point2D {2*1024.f, 768.f}))
                                    << "Cursor position outside the window is wrongly classified as inside the viewport";

        ASSERT_FALSE(isViewportEvent(viewport, common::Point2D {-1.f, -100.f}))
                                    << "Cursor position outside window is wrongly classified as inside the viewport";
    }


    TEST_F(ViewportTest, SharedCamera) {
        mv::common::Environment env{};
        ASSERT_TRUE(env.isUnitTestingInProgress());
        auto& v = viewer::ViewerFactory{}.getViewer();
        Viewport viewport({{0.0f, 0.0f}, {1.0f, 1.0f}});
        viewport.enableGradientBackground(false);
        Mock3DDrawable d1, d2;
        viewport.add(d1);
        viewport.add(d2);
        viewport.render();
        ASSERT_EQ(d1.getCamera()->getId(), d2.getCamera()->getId());
    }

    TEST_F(ViewportTest, WindowToViewportCoordinates) {
        auto& v = viewer::ViewerFactory{}.getViewer();
        Viewport viewport({{0.f, 0.f}, {1.0f, 1.0f}});
        viewport.notifyDisplayResized({1024, 768, 1024, 768});
        auto windowToViewport = viewport.getWindowToViewportTransform();
        common::Point2D windowOrigin = {0, 0};
        auto viewportCoordinate = windowToViewport * windowOrigin;
        ASSERT_NEAR(viewportCoordinate.x, 0.f, 1e-6) << "Window origin doesn't map to viewport top left";
        ASSERT_NEAR(viewportCoordinate.y, 1.f, 1e-6) << "Window origin doesn't map to viewport top left";
        common::Point2D windowLowerLeft = {0, 768};
        viewportCoordinate = windowToViewport * windowLowerLeft;
        ASSERT_NEAR(viewportCoordinate.x, 0.f, 1e-6) << "Window lower left corner doesn't map to viewport origin";
        ASSERT_NEAR(viewportCoordinate.y, 0.f, 1e-6) << "Window lower left corner doesn't map to viewport origin";
        common::Point2D windowLowerRight = {1024, 768};
        viewportCoordinate = windowToViewport * windowLowerRight;
        ASSERT_NEAR(viewportCoordinate.x, 1.f, 1e-6) << "Window lower left corner doesn't map to viewport origin";
        ASSERT_NEAR(viewportCoordinate.y, 0.f, 1e-6) << "Window lower left corner doesn't map to viewport origin";
        common::Point2D windowUpperRight = {1024, 0};
        viewportCoordinate = windowToViewport * windowUpperRight;
        ASSERT_NEAR(viewportCoordinate.x, 1.f, 1e-6) << "Window lower left corner doesn't map to viewport origin";
        ASSERT_NEAR(viewportCoordinate.y, 1.f, 1e-6) << "Window lower left corner doesn't map to viewport origin";
    }

    TEST_F(ViewportTest, ViewportToDeviceCoordinates) {
        auto& v = viewer::ViewerFactory{}.getViewer();
        Viewport viewport({{0.f, 0.f}, {0.5f, 0.5f}});
        viewport.notifyDisplayResized({1024, 768, 1024, 768});
        auto viewportToDevice = viewport.getViewportToDeviceTransform();
        common::Point2D viewportOrigin = {0, 0};
        auto deviceCoordinate = viewportToDevice * viewportOrigin;
        ASSERT_NEAR(deviceCoordinate.x, -1.f, 1e-6) << "Viewport origin doesn't map to near plane bottom left";
        ASSERT_NEAR(deviceCoordinate.y, -1.f, 1e-6) << "Viewport origin doesn't map to near plane bottom left";
        ASSERT_NEAR(deviceCoordinate.z, -1.f, 1e-6) << "Viewport origin doesn't map to near plane bottom left";
        common::Point2D viewportUpperRight = {0.5f, 0.5f};
        deviceCoordinate = viewportToDevice * viewportUpperRight;
        ASSERT_NEAR(deviceCoordinate.x, 1.f, 1e-6) << "Viewport upper right doesn't map to near plane top right";
        ASSERT_NEAR(deviceCoordinate.y, 1.f, 1e-6) << "Viewport upper right doesn't map to near plane top right";
        ASSERT_NEAR(deviceCoordinate.z, -1.f, 1e-6) << "Viewport upper right doesn't map to near plane top right";
    }

}