#pragma once

#include "3dmath/IdentityMatrix.h"


namespace mv::common {
    // Convenience class that allows application code to create and initialize a transform matrix to identity
    // Main use case for creating a separate type is to allow assignments. In  other words, if TransformMatrix was
    // simply a type alias to IdentityMatrix, then assignment of Matrix<T> to a TransformMatrix won't work.
    struct TransformMatrix : math3d::IdentityMatrix<float, 4, 4> {
        TransformMatrix() : math3d::IdentityMatrix<float, 4, 4>() {}
        TransformMatrix& operator=(math3d::Matrix<float, 4, 4> const& another) {
            math3d::Matrix<float, 4, 4>::operator=(another);
            return *this;
        }
    };
}