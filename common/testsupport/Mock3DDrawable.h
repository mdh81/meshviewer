#pragma once

#include "Drawable.h"
#include "gmock/gmock.h"

namespace mv {

    class Mock3DDrawable : public Drawable {
    public:
        MOCK_METHOD(void, render, ());
        Mock3DDrawable() : Drawable("", "", nullptr, mv::Drawable::Effect::None) {
        }
        MOCK_METHOD(void, writeToFile, (std::string, glm::mat4));
        bool is3D() const override { return true; };
    protected:
        MOCK_METHOD(void, generateRenderData, ());
        MOCK_METHOD(void, generateColors, ());
        MOCK_METHOD(common::Point3D, getCentroid, (), (const));
        MOCK_METHOD(common::Bounds, getBounds, (), (const));
    };

}
