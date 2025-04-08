#pragma once

#include <vector>

#include <glad/glad.h>
#include <data/Vertex.h>

class ShaderPipeline;

namespace builder {
	struct Face;
}

struct Tag;

struct Attributes {
	Attributes() = default;
	Attributes(GLint drawType);

	void bind(ShaderPipeline* pipeline);
	void reserve(int size) { m_data.reserve(size); }

	template <typename... Vals>
	inline void add(Vals&&... data) {
		m_data.emplace_back(data...);
		m_elements++;
	}

	void addFace(builder::Face& f);
	void move(data::Vertex&& vert);

	size_t length() const { return m_elements; }

	std::vector<data::Vertex> m_data;

private:
	unsigned int m_buffer = 0;
	GLint m_drawType;
	int m_elements;
};