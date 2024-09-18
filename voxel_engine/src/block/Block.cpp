#pragma once

#include "Block.h"

namespace data {

	Block::Block(BlockID id, const char* name, builder::Geometry* geo) :
		m_id(id),
		m_name(name),
		m_isOpaque(false),
		m_isSolid(false),
		m_geometry(geo)
	{

	}
}