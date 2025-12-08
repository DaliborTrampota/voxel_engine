#pragma once

#include <glm/glm.hpp>

#include <concepts>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>


namespace engine {

    namespace RegistryStoragePolicy {
        struct ByValue {};
        struct ByPointer {};
    };  // namespace RegistryStoragePolicy

    template <typename T>
    concept HasID = requires(T t) {
        { t.getID() } -> std::convertible_to<unsigned int>;
    };

    template <HasID T, typename StoragePolicy = RegistryStoragePolicy::ByValue>
    class Registry;

    template <HasID T>
    class Registry<T, RegistryStoragePolicy::ByValue> {
        friend class RegistryManager;

      public:
        void add(const T& data, const std::string& name) {
            unsigned int id = data.getID();
            if (m_data.find(id) != m_data.end()) {
                throw std::runtime_error("Data with this ID already exists");
            }
            if (m_nameToID.find(name) != m_nameToID.end()) {
                throw std::runtime_error("Data with this name already exists");
            }
            m_data.emplace(id, data);
            m_nameToID.emplace(name, id);
        }

        void add(T&& data, const std::string& name) {
            unsigned int id = data.getID();
            if (m_data.find(id) != m_data.end()) {
                throw std::runtime_error("Data with this ID already exists");
            }
            if (m_nameToID.find(name) != m_nameToID.end()) {
                throw std::runtime_error("Data with this name already exists");
            }
            m_data.emplace(id, std::move(data));
            m_nameToID.emplace(name, id);
        }

        const T& get(unsigned int id) const { return m_data.at(id); }

        const T& get(const char* name) const { return m_data.at(m_nameToID.at(name)); }

      protected:
        std::unordered_map<unsigned int, T> m_data;
        std::unordered_map<std::string, unsigned int> m_nameToID;

      private:
        Registry() = default;
    };


    template <HasID T>
    class Registry<T, RegistryStoragePolicy::ByPointer> {
        friend class RegistryManager;

      public:
        template <typename Derived>
            requires std::derived_from<std::decay_t<Derived>, T>
        void add(Derived&& data, const std::string& name) {
            unsigned int id = data.getID();
            if (m_data.find(id) != m_data.end()) {
                throw std::runtime_error("Data with this ID already exists");
            }
            if (m_nameToID.find(name) != m_nameToID.end()) {
                throw std::runtime_error("Data with this name already exists");
            }
            m_data.emplace(
                id, std::make_unique<std::decay_t<Derived>>(std::forward<Derived>(data))
            );
            m_nameToID.emplace(name, id);
        }

        const T* get(unsigned int id) const {
            auto it = m_data.find(id);
            return it != m_data.end() ? it->second.get() : nullptr;
        }

        const T* get(const char* name) const {
            auto it = m_nameToID.find(name);
            return it != m_nameToID.end() ? get(it->second) : nullptr;
        }

      protected:
        std::unordered_map<unsigned int, std::unique_ptr<T>> m_data;
        std::unordered_map<std::string, unsigned int> m_nameToID;

      private:
        Registry() = default;
    };
}  // namespace engine
