#pragma once

#include "ComponentBase.h"

#include "gloo/Material.h"

namespace GLOO {

    class MaterialComponent : public ComponentBase
    {
    public:
        MaterialComponent(std::shared_ptr<Material> material)
        {
            SetMaterial(std::move(material));
        }

        void SetMaterial(std::shared_ptr<Material> material)
        {
            material_ = std::move(material);
        }

        Material &GetMaterial() { return *material_; }

    private:
        std::shared_ptr<Material> material_;
    };

    CREATE_COMPONENT_TRAIT(MaterialComponent, ComponentType::Material);

}  // namespace GLOO
