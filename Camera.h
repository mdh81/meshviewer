#ifndef CAMERA_H
#define CAMERA_H

#include <memory>

namespace meshviewer {

class Mesh;
    
class Camera {
    
    public:
        enum class ProjectionType {
            Orthographic,
            Perspective
        };

    public:
        Camera(const Mesh&, const ProjectionType proj = ProjectionType::Perspective);
        ~Camera() = default;
    
    
    private:
        class Projection {
            public:
                Projection(const Mesh&);
                virtual void setupView() = 0;
                void fitViewToModel();
                virtual ~Projection() = default;
            private:
                const Mesh& m_mesh;
        };
        
        class PerspectiveProjection : public Projection {
            public:
                PerspectiveProjection(const Mesh&);
                void setupView();
        };

        class OrthographicProjection : public Projection {
            public:
                OrthographicProjection(const Mesh&);
                void setupView();
        };

    private:
        std::unique_ptr<Projection> m_projection;
};

}

#endif
