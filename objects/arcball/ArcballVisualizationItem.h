#pragma once
#pragma once

#include "Drawable3D.h"
#include "3dmath/OrthographicProjectionMatrix.h"

namespace mv::objects {

    class ArcballVisualizationItem : public Drawable3D {
    public:

        ArcballVisualizationItem(std::string const& vertexShaderFileName, std::string const& fragmentShaderFileName);

        [[nodiscard]]
        common::Point3D getCentroid() const override {
            return {};
        }

        [[nodiscard]]
        common::Bounds getBounds() const override {
            return {};
        }

        void render() override;

        void notifyWindowResized(unsigned int windowWidth, unsigned int windowHeight) override;

        // Set the aspect ratio explicitly. This is for supporting the deferred creation
        // use case
        void setInitialProjection(float aspectRatio);

    private:
        void updateProjectionMatrix();

    protected:
        void setTransforms() override;
        virtual void fadeOut() = 0;

    protected:
        math3d::OrthographicProjectionMatrix<float> projectionMatrix;
        unsigned numConnectivityEntries{};
        float opacity;
    };

}