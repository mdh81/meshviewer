#pragma once

#include "Renderable.h"
#include "gmock/gmock.h"

namespace mv {

    class MockRenderable : public Renderable {
    public:
        MOCK_METHOD(void, render, (Camera const&));
        MockRenderable() : Renderable("", "") {
        }
    protected:
        MOCK_METHOD(void, generateRenderData, ());
        MOCK_METHOD(void, generateColors, ());
    };

}
