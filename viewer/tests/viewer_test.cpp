#include "gtest/gtest.h"
#include "ViewerFactory.h"
#include "EventHandler.h"
#include <memory>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::events;

TEST(Viewer, ConvertViewportToWindowCoordinates) {
    auto& v = viewer::ViewerFactory{}.getViewer();
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
