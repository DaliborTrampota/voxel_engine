#include "BlockState.h"

#include <istream>
#include <ostream>

using namespace engine;

BlockState BlockState::makeRotation(glm::vec3 facing) {
    BlockState state;
    state.m_facing = facing;
    return state;
}

void BlockState::serialize(std::ostream& out) const {
    // Serialize base members
    out << m_facing.x << " " << m_facing.y << " " << m_facing.z << " ";
    out << m_rotation << " ";

    // Serialize properties map
    out << m_properties.size() << " ";
    for (const auto& [name, value] : m_properties) {
        out << name << " ";

        // Write variant type index
        out << value.index() << " ";

        // Write value based on type
        std::visit(
            [&out](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, bool>) {
                    out << (val ? 1 : 0) << " ";
                } else if constexpr (std::is_same_v<T, int>) {
                    out << val << " ";
                } else if constexpr (std::is_same_v<T, unsigned int>) {
                    out << val << " ";
                } else if constexpr (std::is_same_v<T, float>) {
                    out << val << " ";
                } else if constexpr (std::is_same_v<T, glm::vec3>) {
                    out << val.x << " " << val.y << " " << val.z << " ";
                } else if constexpr (std::is_same_v<T, glm::ivec3>) {
                    out << val.x << " " << val.y << " " << val.z << " ";
                }
            },
            value
        );
    }
}

void BlockState::deserialize(std::istream& in) {
    // Deserialize base members
    in >> m_facing.x >> m_facing.y >> m_facing.z;
    in >> m_rotation;

    // Deserialize properties map
    size_t propCount;
    in >> propCount;

    m_properties.clear();
    for (size_t i = 0; i < propCount; ++i) {
        std::string name;
        in >> name;

        size_t typeIndex;
        in >> typeIndex;

        // Read value based on type index
        switch (typeIndex) {
            case 0: {  // bool
                int boolVal;
                in >> boolVal;
                m_properties[name] = static_cast<bool>(boolVal);
                break;
            }
            case 1: {  // int
                int val;
                in >> val;
                m_properties[name] = val;
                break;
            }
            case 2: {  // unsigned int
                unsigned int val;
                in >> val;
                m_properties[name] = val;
                break;
            }
            case 3: {  // float
                float val;
                in >> val;
                m_properties[name] = val;
                break;
            }
            case 4: {  // glm::vec3
                glm::vec3 val;
                in >> val.x >> val.y >> val.z;
                m_properties[name] = val;
                break;
            }
            case 5: {  // glm::ivec3
                glm::ivec3 val;
                in >> val.x >> val.y >> val.z;
                m_properties[name] = val;
                break;
            }
        }
    }
}