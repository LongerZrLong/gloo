#include "PhongShader.h"

#include <stdexcept>

#include "gloo/components/CameraComponent.h"
#include "gloo/components/LightComponent.h"
#include "gloo/components/RenderingComponent.h"
#include "gloo/components/MaterialComponent.h"
#include "gloo/SceneNode.h"
#include "gloo/lights/AmbientLight.h"
#include "gloo/lights/PointLight.h"
#include "gloo/lights/DirectionalLight.h"

namespace GLOO {

    PhongShader::PhongShader()
            : ShaderProgram(std::unordered_map<GLenum, std::string>{
            {GL_VERTEX_SHADER,   "phong.vert"},
            {GL_FRAGMENT_SHADER, "phong.frag"}})
    {
    }

    void PhongShader::AssociateVertexArray(VertexArray &vertex_array) const
    {
        if (!vertex_array.HasPositionBuffer())
        {
            throw std::runtime_error("Phong shader requires vertex positions!");
        }
        if (!vertex_array.HasNormalBuffer())
        {
            throw std::runtime_error("Phong shader requires vertex normals!");
        }
        vertex_array.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
        vertex_array.LinkNormalBuffer(GetAttributeLocation("vertex_normal"));
        if (vertex_array.HasTexCoordBuffer())
        {
            vertex_array.LinkTexCoordBuffer(GetAttributeLocation("vertex_tex_coord"));
        }
    }

    void PhongShader::SetTargetNode(const SceneNode &node,
                                    const glm::mat4 &model_matrix) const
    {
        // Associate the right VAO before rendering.
        AssociateVertexArray(node.GetComponentPtr<RenderingComponent>()
                                     ->GetVertexObjectPtr()
                                     ->GetVertexArray());

        // Set transform.
        glm::mat3 normal_matrix =
                glm::transpose(glm::inverse(glm::mat3(model_matrix)));
        SetUniform("model_matrix", model_matrix);
        SetUniform("normal_matrix", normal_matrix);

        // Set material.
        MaterialComponent *material_component_ptr =
                node.GetComponentPtr<MaterialComponent>();
        const Material *material_ptr;
        if (material_component_ptr == nullptr)
        {
            material_ptr = &Material::GetDefault();
        } else
        {
            material_ptr = &material_component_ptr->GetMaterial();
        }
        SetUniform("material.ambient", material_ptr->GetAmbientColor());
        SetUniform("material.diffuse", material_ptr->GetDiffuseColor());
        SetUniform("material.specular", material_ptr->GetSpecularColor());
        SetUniform("material.shininess", material_ptr->GetShininess());

        if (material_ptr->GetAmbientTexture() != nullptr)
        {
            SetUniform("ambient_texture_flag", true);
            SetUniform("ambient_texture", 1);
            material_ptr->GetAmbientTexture()->BindToUnit(1);
        } else
        {
            SetUniform("ambient_texture_flag", false);
        }

        if (material_ptr->GetDiffuseTexture() != nullptr)
        {
            SetUniform("diffuse_texture_flag", true);
            SetUniform("diffuse_texture", 2);
            material_ptr->GetDiffuseTexture()->BindToUnit(2);
        } else
        {
            SetUniform("diffuse_texture_flag", true);
        }

        if (material_ptr->GetSpecularTexture() != nullptr)
        {
            SetUniform("specular_texture_flag", true);
            SetUniform("specular_texture", 3);
            material_ptr->GetSpecularTexture()->BindToUnit(3);
        } else
        {
            SetUniform("specular_texture_flag", false);
        }
    }

    void PhongShader::SetCamera(const CameraComponent &camera) const
    {
        SetUniform("view_matrix", camera.GetViewMatrix());
        SetUniform("projection_matrix", camera.GetProjectionMatrix());
        SetUniform("camera_position",
                   camera.GetNodePtr()->GetTransform().GetWorldPosition());
    }

    void PhongShader::SetLightSource(const LightComponent &component) const
    {
        auto light_ptr = component.GetLightPtr();
        if (light_ptr == nullptr)
        {
            throw std::runtime_error("Light component has no light attached!");
        }

        // First disable all lights.
        // In a single rendering pass, only one light of one type is enabled.
        SetUniform("ambient_light.enabled", false);
        SetUniform("point_light.enabled", false);
        SetUniform("directional_light.enabled", false);

        if (light_ptr->GetType() == LightType::Ambient)
        {
            auto ambient_light_ptr = static_cast<AmbientLight *>(light_ptr);
            SetUniform("ambient_light.enabled", true);
            SetUniform("ambient_light.ambient", ambient_light_ptr->GetAmbientColor());
        } else if (light_ptr->GetType() == LightType::Point)
        {
            auto point_light_ptr = static_cast<PointLight *>(light_ptr);
            SetUniform("point_light.enabled", true);
            SetUniform("point_light.position",
                       component.GetNodePtr()->GetTransform().GetPosition());
            SetUniform("point_light.diffuse", point_light_ptr->GetDiffuseColor());
            SetUniform("point_light.specular", point_light_ptr->GetSpecularColor());
            SetUniform("point_light.attenuation", point_light_ptr->GetAttenuation());
        } else if (light_ptr->GetType() == LightType::Directional)
        {
            auto directional_light_ptr = static_cast<DirectionalLight *>(light_ptr);
            SetUniform("directional_light.enabled", true);
            SetUniform("directional_light.direction",
                       directional_light_ptr->GetDirection());
            SetUniform("directional_light.diffuse",
                       directional_light_ptr->GetDiffuseColor());
            SetUniform("directional_light.specular",
                       directional_light_ptr->GetSpecularColor());
        } else
        {
            throw std::runtime_error(
                    "Encountered light type unrecognized by the shader!");
        }
    }

    void PhongShader::SetShadowMapping(
            const Texture &shadow_texture,
            const glm::mat4 &world_to_light_ndc_matrix) const
    {
        shadow_texture.BindToUnit(4);
        SetUniform("shadow_texture", 4);
        SetUniform("world_to_light_ndc_matrix", world_to_light_ndc_matrix);
        SetUniform("draw_shadow", true);
    }

}  // namespace GLOO
