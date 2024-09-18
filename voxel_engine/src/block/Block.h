#pragma once

namespace builder {
	class Geometry;
}

namespace data {

	typedef unsigned int BlockID;
	typedef unsigned int TexID;

	class Block
	{
	public:
		Block(BlockID id, const char* name, builder::Geometry* geo);

		unsigned int getID() const { return m_id; }
		const char* getName() const { return m_name; }

		builder::Geometry* geometry() const { return m_geometry; }

		Block* setOpaque(bool opaque) { m_isOpaque = opaque; return this; }
		Block* setSolid(bool solid) { m_isSolid = solid; return this; }

		Block* setGeometry(builder::Geometry* geometry) { m_geometry = geometry; return this; }

	private:
		const char* m_name;
		BlockID m_id;
		bool m_isOpaque;
		bool m_isSolid;

		builder::Geometry* m_geometry;


	};
}