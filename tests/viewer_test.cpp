#include "gtest/gtest.h"
#include "Viewer.h"
#include "Mesh.h"
#include <memory>
using namespace std;
using namespace meshviewer;
using namespace meshviewer::common;

TEST(Viewer, Singleton) {
    Viewer& v = Viewer::getInstance();
    Viewer& v1 = Viewer::getInstance();
    ASSERT_EQ(&v, &v1) << "Viewer::getInstance() returned more than one singleton instance";
}
