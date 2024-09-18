#include "CulledGeometry.h"
#include "CulledGeometry.h"

namespace builder {

	bool CulledGeometry::sideSpecific()
	{
		return false;
	}
	FaceIterator builder::CulledGeometry::begin() const
	{
		return FaceIterator();
	}

	FaceIterator builder::CulledGeometry::end() const
	{
		return FaceIterator();
	}

}