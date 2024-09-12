#pragma once

#include <glad/glad.h>

#include <vector>


class ShaderPipeline;

struct Attribute {
	Attribute() = default;
	Attribute(int size, GLint drawType);

	void bind(ShaderPipeline* pipeline, int location);
	template <typename... Vals>
	void add(Vals... data) {
		(m_data.push_back(data), ...);
		m_elements++;
	}

	size_t length() const { return m_elements; }
	int size() const { return m_size; }

	std::vector<float> m_data;

private:
	unsigned int m_buffer;
	int m_size;
	GLint m_drawType;

	int m_elements;
};