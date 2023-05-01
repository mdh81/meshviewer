#pragma once

#include "Renderable.h"
#include "gmock/gmock.h"

namespace mv {

    class MockRenderable : public Renderable {
    public:
        MOCK_METHOD(void, render, ());
        MockRenderable() : Renderable("", "") {
        }
    protected:
        MOCK_METHOD(void, generateRenderData, ());
        MOCK_METHOD(void, generateColors, ());
        MOCK_METHOD(common::Point3D, getCentroid, (), (const));
        MOCK_METHOD(common::Bounds, getBounds, (), (const));
    };

}
