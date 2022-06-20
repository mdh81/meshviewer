#include "MeshViewerObject.h"
#include <iostream>

namespace meshviewer {

size_t MeshViewerObject::sm_instanceCount = 0;
    
MeshViewerObject::MeshViewerObject() {
    m_id = ++sm_instanceCount;
}

bool MeshViewerObject::operator==(const MeshViewerObject& another) const {
    if (m_debugOn) {
        std::cout << "Comparing object with id " << m_id << " to another object with id " << another.m_id << std::endl;
    }
    return m_id == another.m_id;
}

}
