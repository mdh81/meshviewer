#pragma once

#include "Drawable.h"

namespace mv {

// A Gadget is a supporting object in the viewer that defines its own view.
// The viewport's camera doesn't operate on gadgets. Example gadgets are the
// GradientBackground and Orientation Marker

class Gadget : public Drawable {

public:
    Gadget(std::string vertexShaderFileName, std::string fragmentShaderFileName)
    : Drawable(vertexShaderFileName, fragmentShaderFileName,
               std::make_shared<Camera>(*this, Camera::ProjectionType::Orthographic),
               Effect::None) { }
};

} // mv
