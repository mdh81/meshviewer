#include "MeshViewerObject.h"
#include <iostream>
using namespace std;

namespace meshviewer {

size_t MeshViewerObject::sm_instanceCount = 0;
    
MeshViewerObject::MeshViewerObject(ostream& os) : m_outputStream(os) {
    m_id = ++sm_instanceCount;
    m_debugOn = false;
}

bool MeshViewerObject::operator==(const MeshViewerObject& another) const {
    if (m_debugOn) {
        m_outputStream << 
            "Comparing object with id " << m_id << " to another object with id " << another.m_id << endl;
    }
    return m_id == another.m_id;
}

}
