#include "gtest/gtest.h"
#include "MockRendererable.h"
#include "Viewport.h"
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

TEST(Viewport, RendererablesMangement) {
    Viewport viewport({0.0f, 0.0f, 1.0f,1.0f});
    mv::MockRenderable renderable1, renderable2;
    viewport.add(renderable1);
    viewport.add(renderable2);
    ASSERT_EQ(viewport.getRenderables().size(), 2) << "Unexpected number of renderables after add";
    ASSERT_TRUE(viewport.getRenderables().at(0).get().getId() == renderable1.getId() ||
                viewport.getRenderables().at(0).get().getId() == renderable2.getId()) << "Unexpected renderable after add";
    ASSERT_TRUE(viewport.getRenderables().at(1).get().getId() == renderable1.getId() ||
                viewport.getRenderables().at(1).get().getId() == renderable2.getId()) << "Unexpected renderable after add";
    viewport.remove(renderable2);
    ASSERT_EQ(viewport.getRenderables().size(), 1) << "Unexpected number of renderables after remove";
    ASSERT_TRUE(viewport.getRenderables().at(0).get().getId() == renderable1.getId()) << "Unexpected renderable after remove";
    ASSERT_THROW({
        try {
            viewport.remove(mv::MockRenderable());
        } catch(std::runtime_error&) {
            throw;
        }
    }, std::runtime_error) << "Expected an exception to be thrown when a non-existent renderer is removed";
}