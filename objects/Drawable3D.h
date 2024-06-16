#pragma once

#include "Drawable.h"

namespace mv {

class Drawable3D : public Drawable {

    public:
        Drawable3D(std::string vertexShaderFileName, std::string fragmentShaderFileName,
                   Effect supportedEffects = Effect::None);

        [[nodiscard]]
        bool is3D() const override { return true; }

    protected:
        void setTransforms() override;

    protected:
        // For building mocks
        Drawable3D() = default;
    };

}
