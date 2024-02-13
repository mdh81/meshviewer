#pragma once

#include "Gadget.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class Arcball : public Gadget {
    public:

        Arcball();

        void render() override;

        [[nodiscard]]
        common::Point3D getCentroid() const override {
            return {0.f, 0.f, 0.f};
        }

        [[nodiscard]]
        common::Bounds getBounds() const override {
            return {{-1.f, +1.f}, {-1.f, +1.f}, {-1.f, +1.f}};
        }

        void notifyWindowResized(unsigned int windowWidth, unsigned int windowHeight) override;

    protected:
        void setTransforms() override;

    protected:
        void generateRenderData() override;

        void generateColors() override;

    private:
        unsigned const sphereResolution;
        common::RGBAColor const sphereColor;
        math3d::OrthographicProjectionMatrix<float> projectionMatrix;
        unsigned numConnectivityEntries{};
    };

}
