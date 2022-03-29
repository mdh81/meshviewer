#ifndef STL_READER_H
#define STL_READER_H
#include <string>
#include <memory>
#include "Mesh.h"

namespace meshviewer {

class STLReader {
    public:
        STLReader(const std::string& fn) : m_fileName(fn) {}             
        STLReader(const STLReader&) = delete;
        STLReader(STLReader&&) = delete;
        STLReader& operator=(const STLReader&) = delete;
        STLReader& operator=(STLReader&&) = delete;
        void getOutput(std::unique_ptr<Mesh>& mesh);
    private:
        void readBinary(std::ifstream& ifs, std::unique_ptr<Mesh>& mesh);
    private:
        std::string m_fileName;
};

}

#endif