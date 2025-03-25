#pragma once

#include "Geometry.h"
#include "Face.h"

namespace builder {

	class CulledGeometry : public Geometry
	{
	public:
		CulledGeometry() = default;

		bool sideSpecific(); //TODO? override;

		FaceIterator begin() const;
		FaceIterator end() const;

	private:
		std::vector<Faces> m_faces;
	};

	struct FaceIterator {
		using category = std::forward_iterator_tag;
		using valueType = Face;
		using differenceType = std::ptrdiff_t;
		using pointer = Face*;
		using reference = Face&;

		FaceIterator(pointer ptr) : m_ptr(ptr) {}

		reference operator*() const { return *m_ptr; }
		pointer operator->() { return m_ptr; }

		// Prefix increment
		FaceIterator& operator++() { m_ptr++; return *this; }

		// Postfix increment
		FaceIterator operator++(int) { FaceIterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const FaceIterator& a, const FaceIterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const FaceIterator& a, const FaceIterator& b) { return a.m_ptr != b.m_ptr; };

	private:
		pointer m_ptr;
	};

}