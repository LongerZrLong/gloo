#pragma once

#include "LightBase.h"

namespace GLOO {

    class AmbientLight : public LightBase
    {
        // Use diffuse_color_ as the ambient light color.
    public:
        void SetAmbientColor(const glm::vec3 &color) { SetDiffuseColor(color); }

        glm::vec3 GetAmbientColor() const { return GetDiffuseColor(); }

        LightType GetType() const override { return LightType::Ambient; }
    };

}  // namespace GLOO

