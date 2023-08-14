#pragma once

#include "Drawable3D.h"

namespace mv {

    class SketchPlane : public Drawable3D {

    // TODO: Implement creation semantics

    SketchPlane(float spacing);

    public:
        void render() override;

        [[nodiscard]] common::Point3D getCentroid() const override;

        [[nodiscard]] common::Bounds getBounds() const override;

        void setResolution(float spacing) { this->spacing = spacing; }

    protected:
        void generateRenderData() override;

        void generateColors() override;

    private:
        float spacing;

    };

}
