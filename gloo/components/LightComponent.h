#pragma once

#include "ComponentBase.h"

#include <glm/glm.hpp>

#include "gloo/lights/LightBase.h"

namespace GLOO {

    class LightComponent : public ComponentBase
    {
    public:
        LightComponent(std::shared_ptr<LightBase> light) : light_(std::move(light)) {};

        LightBase *GetLightPtr() const { return light_.get(); }

        bool CanCastShadow() const
        {
            return (light_->GetType() == LightType::Directional ||
                    light_->GetType() == LightType::Point);
        }

    private:
        std::shared_ptr<LightBase> light_;
    };

    CREATE_COMPONENT_TRAIT(LightComponent, ComponentType::Light);

}  // namespace GLOO
