#include "CulledGeometry.h"

using namespace engine;

bool CulledGeometry::sideSpecific() {
    return false;
}

FaceIterator CulledGeometry::begin() const {
    return FaceIterator((Face*)&m_faces.at(0));
}

FaceIterator CulledGeometry::end() const {
    return FaceIterator((Face*)&m_faces.at(0));
}