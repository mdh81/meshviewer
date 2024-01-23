#include "gtest/gtest.h"
#include "MeshViewerObject.h"
using namespace std;
using namespace mv;

class Mesh : public MeshViewerObject {};
class Octree : public MeshViewerObject {};

TEST(Object, TestIdentifier) {
    Mesh m;
    Octree o;
    ASSERT_NE(m.getId(), o.getId()) << "Two different identifiers were expected" << std::endl;
    ASSERT_EQ(2, o.getId()) << "Wrong identifier" << std::endl;
    ASSERT_EQ(1, m.getId()) << "Wrong identifier" << std::endl;
}

TEST(Object, TestComparison) {
    Mesh m1, m2;
    ASSERT_TRUE(m1 == m1) << "Comparing an object to itself failed" << std::endl;
    ASSERT_TRUE(m2 != m1) << "Comparison of two different objects produced wrong output" << std::endl;
}

TEST(Object, DebugFlag) {
    Mesh m1;
    ASSERT_EQ(m1.isDebugOn(), false) << "Debug flag is unexpectedly on upon construction" << std::endl;
    m1.debugOn();
    ASSERT_EQ(m1.isDebugOn(), true) << "Debug flag not set after calling debugOn" << std::endl;
    m1.debugOff();
    ASSERT_EQ(m1.isDebugOn(), false) << "Debug flag not unset after calling debugOff" << std::endl;
}

