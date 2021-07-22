#include "LightbulbNode.h"
#include <gloo/components/LightComponent.h>

#include <gloo/components/RenderingComponent.h>
#include <gloo/debug/PrimitiveFactory.h>
#include <gloo/shaders/PhongShader.h>
#include <gloo/components/MaterialComponent.h>
#include <gloo/components/ShadingComponent.h>

namespace GLOO {

    LightbulbNode::LightbulbNode() : time_elapsed_(0.0f)
    {
        light_ = std::make_shared<PointLight>();
        light_->SetAttenuation(glm::vec3(0.1f));
        light_->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
        light_->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
        CreateComponent<LightComponent>(light_);

        shader_ = std::make_shared<PhongShader>();
        sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
        material_ = std::make_shared<Material>(Material::GetDefault());

        CreateComponent<RenderingComponent>(sphere_mesh_);
        CreateComponent<MaterialComponent>(material_);
        CreateComponent<ShadingComponent>(shader_);
    }

    void LightbulbNode::Update(double delta_time)
    {
        time_elapsed_ += delta_time;
        glm::vec3 light_dir(2.0f * sinf((float) time_elapsed_ * 1.5f * 0.1f), 5.0f,
                            2.0f * cosf(2 + (float) time_elapsed_ * 1.9f * 0.1f));
        light_dir = glm::normalize(light_dir);
        glm::vec3 eye = 5.0f * light_dir;
        UpdateLightBulb(eye, -light_dir);
    }

    void LightbulbNode::UpdateLightBulb(const glm::vec3 &eye, const glm::vec3 &direction)
    {
        auto up_dir = glm::normalize(
                glm::cross(glm::normalize(direction), glm::vec3(0.0f, 0.0f, 1.0f)));
        auto light_view = glm::lookAt(eye, glm::vec3(0.0f), up_dir);

        auto light_to_world_mat = glm::inverse(light_view);
        GetTransform().SetMatrix4x4(light_to_world_mat);
    }

}  // namespace GLOO
