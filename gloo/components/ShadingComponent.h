#pragma once

#include "ComponentBase.h"

#include <memory>

#include "gloo/shaders/ShaderProgram.h"
#include "gloo/alias_types.h"

namespace GLOO {

    class ShadingComponent : public ComponentBase
    {
    public:
        ShadingComponent(std::shared_ptr<ShaderProgram> shader)
                : shader_(std::move(shader))
        {
        }

        ShaderProgram *GetShaderPtr() { return shader_.get(); }

    private:
        std::shared_ptr<ShaderProgram> shader_;
    };

    CREATE_COMPONENT_TRAIT(ShadingComponent, ComponentType::Shading);

}  // namespace GLOO
