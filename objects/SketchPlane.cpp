
#include "SketchPlane.h"

mv::SketchPlane::SketchPlane(float spacing)
: Drawable3D("SketchPlaneVertex.glsl",
          "SketchPlaneFragment.glsl",
          Effect::Fog)
, spacing(spacing) {

}

void mv::SketchPlane::render() {

}

mv::common::Point3D mv::SketchPlane::getCentroid() const {
    return {0.f, 0.f, 0.f};
}

mv::common::Bounds mv::SketchPlane::getBounds() const {
    // TODO: This plane should cover the entire NDC. Convert 2 units in NDC to
    // world space and return that for X and Z. The plane's Y bounds are 0
    return mv::common::Bounds();
}

void mv::SketchPlane::generateRenderData() {

    // NOTES:
    // 1) The sketch plane should span the NDC in X and Z directions
    // 2) It should use the same view transform as the scene
    // 3) It should use the same projection mode as the scene

    // Is it better to use a different projection transform or use the
    // same one as the scene. I mean the bounds are different--we can
    // make the plane as wide and tall and the model's bounding box, but
    // that sounds wasteful. Better to make it a -1 to +1 in X and Z for
    // simplicity and create a projection matrix that will map it to the
    // extents of NDC


}

void mv::SketchPlane::generateColors() {

}


