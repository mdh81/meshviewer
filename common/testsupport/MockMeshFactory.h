#pragma once

#include "MeshFactory.h"
#include "MockMesh.h"
#include <memory>

namespace mv {

    class MockMeshFactory : public mv::IMeshFactory {
        public:
            mv::Mesh::MeshPointer createMesh() const override {
                return std::make_unique<MockMesh>();
            }
    };

}