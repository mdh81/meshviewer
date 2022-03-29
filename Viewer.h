#ifndef VIEWER_H
#define VIEWER_H
#include "Mesh.h"

class GLFWwindow;

namespace meshviewer {

class Viewer {
    public:
        ~Viewer() = default;
        void displayMesh(const Mesh& mesh);

    // Creation Semantics 
    public:
        static Viewer& getInstance();
    private:
        Viewer(unsigned winWidth=1024, unsigned winHeight=768);
        Viewer(const Viewer&) = delete;
        Viewer(Viewer&&) = delete;
        Viewer& operator=(const Viewer&) = delete;
        Viewer& operator=(Viewer&&) = delete;

    private:
        unsigned m_windowWidth;
        unsigned m_windowHeight;
        GLFWwindow* m_window;
};

}

#endif