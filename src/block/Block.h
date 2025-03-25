#pragma once

#include "block/builder/Geometry.h"
#include <string>


namespace data {

	typedef unsigned int BlockID;
	typedef unsigned int TexID;

	class Block
	{
	public:
		Block(BlockID id, const char* name, builder::Geometry geo);

		unsigned int getID() const { return m_id; }
		const char* getName() const { return m_name.c_str(); }

		const builder::Geometry* geometry() const { return &m_geometry; }
		bool isSolid() const { return m_isSolid; }
		bool isOpaque() const { return m_isOpaque; }
		bool isVoxel() const { return m_isVoxel; }


		Block& isOpaque(bool opaque) { m_isOpaque = opaque; return *this; }
		Block& isSolid(bool solid) { m_isSolid = solid; return *this; }
		Block& isVoxel(bool voxel) { m_isVoxel = voxel; return *this; }

		Block& setGeometry(builder::Geometry&& geometry) { m_geometry = geometry; return *this; }

	private:
		std::string m_name;
		BlockID m_id;
		bool m_isOpaque;
		bool m_isSolid;
		bool m_isVoxel;

		builder::Geometry m_geometry;
		


	};
}