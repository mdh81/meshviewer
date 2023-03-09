#ifndef STL_READER_H
#define STL_READER_H
#include <string>
#include <memory>
#include "Mesh.h"

namespace meshviewer {

class STLReader : public MeshViewerObject {
    public:
        STLReader(const std::string& fn, const bool clean = true) : m_fileName(fn),
                                                                    m_clean(clean) {}    
        STLReader(const STLReader&) = delete;
        STLReader(STLReader&&) = delete;
        STLReader& operator=(const STLReader&) = delete;
        STLReader& operator=(STLReader&&) = delete;
        void getOutput(std::unique_ptr<Mesh>& mesh);
    private:
        void readBinary(std::ifstream& ifs, std::unique_ptr<Mesh>& mesh); 
    private:
        std::string m_fileName;
        bool m_clean;
};

}

#endif
