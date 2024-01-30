#include "MeshViewerObject.h"
#include "ObjectRegistry.h"
#include <iostream>

namespace mv {

size_t MeshViewerObject::instanceCount = 0;
    
MeshViewerObject::MeshViewerObject()
    : id(++instanceCount) {
    ObjectRegistry{}.registerObject(*this);
}

MeshViewerObject::~MeshViewerObject() {
    ObjectRegistry{}.unRegisterObject(*this);
}

MeshViewerObject::MeshViewerObject(MeshViewerObject const& another)
    : id(++instanceCount) {

}

MeshViewerObject::MeshViewerObject(MeshViewerObject&& another)
    : id(another.id) {
}

MeshViewerObject& MeshViewerObject::operator=(MeshViewerObject const&) {
    return *this;
}

MeshViewerObject& MeshViewerObject::operator=(MeshViewerObject&& another) {
    id = another.id;
    return *this;
}

bool MeshViewerObject::operator==(const MeshViewerObject& another) const {
    if (isDebugOn()) {
        std::cout << "Comparing object with id " << id << " to another object with id " << another.id << std::endl;
    }
    return id == another.id;
}

}