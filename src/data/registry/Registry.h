#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <concepts>

#include <unordered_map>

#include "block/builder/Geometry.h"
#include <string>



namespace data {

	template <typename T>
	concept HasID = requires(T t) {
		{ t.getID() } -> std::convertible_to<unsigned int>;
		{ t.getName() } -> std::convertible_to<const char*>;
	};



	template <HasID T>
	class Registry {

	public:
		Registry() = default;

		void add(T data) {
			m_data.emplace(data.getID(), data);
			m_nameToID.emplace(data.getName(), data.getID());
		}

		const T& get(unsigned int id) const {
			return m_data.at(id);
		}

		const T& get(const char* name) const {
			return m_data.at(m_nameToID.at(name));
		}




	protected:
		std::unordered_map<unsigned int, T> m_data;
		std::unordered_map<std::string, unsigned int> m_nameToID;
	};


	//class GeometryRegistry {

	//public:
	//	GeometryRegistry() = default;

	//	void add(const char* name, builder::Geometry* geo) {
	//		m_data.emplace(name, geo);
	//	}

	//	builder::Geometry* get(const char* name) {
	//		return m_data.at(name);
	//	}

	//private:
	//	std::unordered_map<const char*, builder::Geometry*> m_data;
	//};


}
namespace registry {
	void Init();
	void Clean();
	//void InitGeometries();
	void InitBlocks();
}