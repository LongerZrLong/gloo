#pragma once

#include "LightBase.h"

namespace GLOO {

    class DirectionalLight : public LightBase
    {
    public:
        void SetDirection(const glm::vec3 &direction)
        {
            direction_ = glm::normalize(direction);
        }

        glm::vec3 GetDirection() const { return direction_; }

        LightType GetType() const override { return LightType::Directional; }

    private:
        glm::vec3 direction_;
    };

}  // namespace GLOO
