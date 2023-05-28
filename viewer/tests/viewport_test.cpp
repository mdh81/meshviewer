#include "gtest/gtest.h"
#include "MockDrawable.h"
#include "../Viewport.h"
#include "3dmath/Vector.h"
using namespace std;
using namespace mv::scene;

TEST(Viewport, Creation) {
    Viewport viewport({0.0f, 0.0f, 1.0f,1.0f});
    ASSERT_FLOAT_EQ(viewport.getOrigin().x, 0.0f) << "Origin x-coordinate is incorrect";
    ASSERT_FLOAT_EQ(viewport.getOrigin().y, 0.0f) << "Origin y-coordinate is incorrect";
    ASSERT_FLOAT_EQ(viewport.getWidth(), 1.0f) << "Width is incorrect";
    ASSERT_FLOAT_EQ(viewport.getHeight(), 1.0f) << "Height is incorrect";
}

TEST(Viewport, DrawablesManagement) {
    Viewport viewport({0.0f, 0.0f, 1.0f,1.0f});
    mv::MockDrawable drawable1, drawable2;
    viewport.add(drawable1);
    viewport.add(drawable2);
    ASSERT_EQ(viewport.getDrawables().size(), 2) << "Unexpected number of renderables after add";
    ASSERT_TRUE(viewport.getDrawables().at(0).get().getId() == drawable1.getId() ||
                viewport.getDrawables().at(0).get().getId() == drawable2.getId()) << "Unexpected renderable after add";
    ASSERT_TRUE(viewport.getDrawables().at(1).get().getId() == drawable1.getId() ||
                viewport.getDrawables().at(1).get().getId() == drawable2.getId()) << "Unexpected renderable after add";
    viewport.remove(drawable2);
    ASSERT_EQ(viewport.getDrawables().size(), 1) << "Unexpected number of renderables after remove";
    ASSERT_TRUE(viewport.getDrawables().at(0).get().getId() == drawable1.getId()) << "Unexpected renderable after remove";
    ASSERT_THROW({
        try {
            auto drawable = mv::MockDrawable();
            viewport.remove(drawable);
        } catch(std::runtime_error&) {
            throw;
        }
    }, std::runtime_error) << "Expected an exception to be thrown when a non-existent drawable is removed";
}