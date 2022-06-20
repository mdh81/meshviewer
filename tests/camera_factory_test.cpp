#include "gtest/gtest.h"
#include "CameraFactory.h"
using namespace meshviewer;

TEST(CameraFactory, Singleton) {
    CameraFactory& cf1 = CameraFactory::getInstance(); 
    CameraFactory& cf2 = CameraFactory::getInstance(); 
    ASSERT_EQ(&cf1, &cf2) << "CameraFactory::getInstance() is not returning singleton" << std::endl;
}

TEST(CameraFactory, GetCamera) {
    Mesh m;
    CameraFactory& cf = CameraFactory::getInstance();
    // Call twice with the same parameters
    Camera& c1 = cf.getCamera(m, CameraFactory::ProjectionType::Perspective);
    Camera& c2 = cf.getCamera(m, CameraFactory::ProjectionType::Perspective);
    ASSERT_EQ(&c1, &c2) << "Expected same camera for same combination of mesh and projection type" << std::endl;
    
    // Change projection type 
    Camera& c3 = cf.getCamera(m, CameraFactory::ProjectionType::Orthographic);
    ASSERT_NE(&c1, &c3) << "Expected different camera instance for new projection type" << std::endl;

    // Change mesh
    Camera& c4 = cf.getCamera(Mesh(), CameraFactory::ProjectionType::Orthographic);
    ASSERT_NE(&c4, &c3) << "Expected different camera instance for new mesh" << std::endl;
}
