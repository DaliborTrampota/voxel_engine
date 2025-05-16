#pragma once

#include <glm/glm.hpp>

#include <concepts>
#include <string>
#include <unordered_map>
#include <stdexcept>



namespace engine {

	template <typename T>
	concept HasID = requires(T t) {
		{ t.getID() } -> std::convertible_to<unsigned int>;
	};



	template <HasID T>
	class Registry {
		friend class RegistryManager;

	public:
		void add(const T& data, const std::string& name) {
            if (m_data.find(data.getID()) != m_data.end()) {
                throw std::runtime_error("Data with this ID already exists");
            }
            m_data.emplace(data.getID(), data);
            if (m_nameToID.find(name) != m_nameToID.end()) {
                throw std::runtime_error("Data with this name already exists");
            }
			m_nameToID.emplace(name, data.getID());
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

    private:    
		Registry() = default;
	};
}