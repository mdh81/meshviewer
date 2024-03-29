#pragma once

#include "Types.h"
#include "Drawable3D.h"
#include <optional>
#include <memory>

namespace mv {

// TODO: Nest inside namespace objects

class Mesh;
class Camera;

class Glyph : public Drawable3D {
    // TODO: Define creation semantics
    public:
        Glyph(Mesh const&, common::GlyphAssociation);
        void render() override;

    [[nodiscard]] common::Point3D getCentroid() const override;

    [[nodiscard]] common::Bounds getBounds() const override;

protected:
        void generateRenderData() override;
        void generateColors() override;
    private:
        size_t buildFaceNormalData();
        size_t buildVertexNormalData();
    private:
        common::GlyphAssociation m_association;
        Mesh const& m_mesh;
        std::unique_ptr<float[]> m_vertexData;
        std::unique_ptr<unsigned[]> m_elementData;
        size_t m_numGlyphs;
};

}
