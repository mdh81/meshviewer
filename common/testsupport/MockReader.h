#pragma once

#include "Reader.h"
#include "gmock/gmock.h"
#include "Mesh.h"
#include "MockMeshFactory.h"
#include "MockMesh.h"

namespace mv {

    struct MockReader : readers::Reader {
        MOCK_METHOD(mv::Mesh::MeshPointer, getOutput, (Mesh::MeshPointer));

        MockReader(std::string = "") : readers::Reader("", MockMeshFactory{}) {
            ON_CALL(*this, getOutput).WillByDefault([](Mesh::MeshPointer) {
                return std::make_unique<MockMesh>();
            });
        }
    };

}