#pragma once

#include "Drawable3D.h"
#include "gmock/gmock.h"

namespace mv {

    class Mock3DDrawable : public Drawable3D {
    public:
        MOCK_METHOD(void, render, ());
        Mock3DDrawable() : Drawable3D("", "", mv::Drawable::Effect::None) {
        }
        MOCK_METHOD(void, writeToFile, (std::string, glm::mat4));
    protected:
        MOCK_METHOD(void, setTransforms, (), (override));
        MOCK_METHOD(void, generateRenderData, ());
        MOCK_METHOD(void, generateColors, ());
        MOCK_METHOD(common::Point3D, getCentroid, (), (const));
        MOCK_METHOD(common::Bounds, getBounds, (), (const));
    };

}
