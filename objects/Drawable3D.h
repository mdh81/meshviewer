#pragma once

#include "Drawable.h"
#include "Camera.h"

namespace mv {

class Drawable3D : public Drawable {

public:
    Drawable3D(std::string vertexShaderFileName, std::string fragmentShaderFileName, Effect supportedEffects = Effect::None)
    : Drawable(vertexShaderFileName, fragmentShaderFileName, nullptr, supportedEffects) {}

    [[nodiscard]]
    bool is3D() const override { return true; }

    virtual void writeToFile(std::string const& fileName, common::TransformMatrix const& transform) const override {
        std::cerr << std::string(__PRETTY_FUNCTION__) + " is not implemented" << std::endl;
    }

    };



} // mv
