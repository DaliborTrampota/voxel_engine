#pragma once

#include <glm/glm.hpp>

#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>


namespace engine {

    class BlockState {
      public:
        using Value = std::variant<bool, int, unsigned int, float, glm::vec3>;

      private:
        // Helper to check if type T is one of the types in the variant (100% AI code :) )
        template <typename T, typename Variant>
        struct is_variant_member;

        template <typename T, typename... Types>
        struct is_variant_member<T, std::variant<Types...>>
            : std::disjunction<std::is_same<T, Types>...> {};

        template <typename T>
        static constexpr bool is_value_type_v = is_variant_member<T, Value>::value;

      public:
        static BlockState& Empty() {
            static BlockState empty = BlockState();
            return empty;
        }
        static BlockState make(glm::vec3 orientation);

        BlockState() = default;
        ~BlockState() = default;

        template <typename T>
            requires is_value_type_v<T>
        void set(const std::string& name, const T& value) {
            m_properties[name] = value;
        }

        template <typename T>
            requires is_value_type_v<T>
        T get(const std::string& name) const {
            auto it = m_properties.find(name);
            if (it == m_properties.end()) {
                return T();
            }
            return std::get<T>(it->second);
        }

      protected:
        glm::vec3 m_facing;
        /// @note rotation around facing axis
        float m_rotation;
        uint8_t m_variant = 0;

        std::unordered_map<std::string, Value> m_properties;
    };
}  // namespace engine