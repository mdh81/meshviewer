#include "gtest/gtest.h"
#include "CameraFactory.h"
using namespace mv;

TEST(CameraFactory, Singleton) {
    CameraFactory& cf1 = CameraFactory::getInstance(); 
    CameraFactory& cf2 = CameraFactory::getInstance(); 
    ASSERT_EQ(&cf1, &cf2) << "CameraFactory::getInstance() is not returning singleton" << std::endl;
}

TEST(CameraFactory, GetCamera) {
    Mesh m;
    CameraFactory& cf = CameraFactory::getInstance();
    
    // Call twice with the same parameters
    Camera& c1 = cf.getCamera(m, {0,0});
    Camera& c2 = cf.getCamera(m, {0,0});
    ASSERT_EQ(&c1, &c2) << "Expected same camera for the same mesh" << std::endl;
    
    // Change mesh
    Camera& c4 = cf.getCamera(Mesh(), {0,0});
    ASSERT_NE(&c1, &c4) << "Expected different camera instance for new mesh" << std::endl;
}
