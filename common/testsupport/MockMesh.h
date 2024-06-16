#pragma once

#include "Mesh.h"
#include "Mock3DDrawable.h"
#include "Vertex.h"
#include "gmock/gmock.h"

namespace mv {
    class MockMesh : public Mesh {
        public:
            MOCK_METHOD(void, initialize, (unsigned numVertices, unsigned numFaces), (override));
            MOCK_METHOD(void, addVertex, (float x, float y, float z), (override));
            MOCK_METHOD(void, addFace, (const std::initializer_list<unsigned>& vertexIds), (override));
            MOCK_METHOD(unsigned, removeDuplicateVertices, (), (override));
            MOCK_METHOD(unsigned, getNumberOfVertices, (), (const, override));
            MOCK_METHOD(unsigned, getNumberOfFaces, (), (const, override));
            MOCK_METHOD(const Vertices&, getVertices, (), (const, override));
            MOCK_METHOD(const Faces&, getConnectivity, (), (const, override));
            MOCK_METHOD(const mv::Vertex&, getVertex, (unsigned vertexIndex), (const, override));
            MOCK_METHOD(const mv::Face&, getFace, (unsigned faceIndex), (const, override));
            MOCK_METHOD(VertexData, getVertexData,(), (const, override));
            MOCK_METHOD(void, getConnectivityData,(size_t & numBytes, unsigned * &connData), (const, override));
            MOCK_METHOD(std::unique_ptr<Mesh>, transform,(common::TransformMatrix const &transformMatrix), (const, override));
            MOCK_METHOD(void, writeToSTL, (std::string const&), (const, override));
            MOCK_METHOD(NormalData, getNormals,(common::NormalLocation), (const, override));
            MOCK_METHOD(void, generateRenderData, (), (override));
            MOCK_METHOD(void, generateColors, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(common::Point3D, getCentroid, (), (const, override));
            MOCK_METHOD(common::Bounds, getBounds, (),  (const, override));
    };
    // Drawable::setTransforms is pure virtual just like getBounds, getCentroid and
    // other inherited methods, but since Drawable3D overrides this method, the linker
    // expects to find a definition of Drawable3D::setTransforms, and ignores
    // MockMesh::setTransforms(). Therefore, provide a dummy definition for this method
    // to satisfy linker requirements
    void Drawable3D::setTransforms() {
    }
}