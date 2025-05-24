#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace engine {

    class IEntity {
        public:
            IEntity() = default;
            virtual ~IEntity() = default;
    
            virtual void update(float dt) = 0;
    
            virtual void position(const glm::vec3& position) { m_position = position; }
            virtual const glm::vec3& position() const { return m_position; }
    
        protected:
            glm::vec3 m_position{0.0f, 0.0f, 0.0f};
    };
}