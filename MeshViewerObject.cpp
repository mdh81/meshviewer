#include "MeshViewerObject.h"

namespace meshviewer {

size_t MeshViewerObject::sm_instanceCount = 0;
    
MeshViewerObject::MeshViewerObject() {
    m_id = ++sm_instanceCount;
}

bool MeshViewerObject::operator==(const MeshViewerObject& another) const {
    return m_id == another.m_id;
}

}