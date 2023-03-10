#ifndef GLYPH_H
#define GLYPH_H

#include "Types.h"
#include "Renderable.h"
#include <optional>
#include <memory>

namespace mv {

class Mesh;
class Camera;

class Glyph : public Renderable {
    // TODO: Define creation semantics
    public:
        Glyph(Mesh const&, const common::GlyphAssociation);
        void render(Camera const&) override;
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
#endif
