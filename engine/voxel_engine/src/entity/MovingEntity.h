#pragma once

#include "IEntity.h"

namespace engine {

    class MovingEntity : public IEntity {
        public:
            MovingEntity();
            //~MovingEntity() override;

            void update(float dt) override;

            void move(const glm::vec3& direction);

        protected:
            float m_speed = 10.f;
            glm::vec3 m_velocity{0.0f, 0.0f, 0.0f};
            glm::vec3 m_acceleration{0.0f, 0.0f, 0.0f};
    };
}