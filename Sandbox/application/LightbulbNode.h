#pragma once

#include <gloo/VertexObject.h>

#include <gloo/shaders/ShaderProgram.h>
#include <gloo/Material.h>
#include <gloo/SceneNode.h>

#include <gloo/lights/PointLight.h>

namespace GLOO {

    class LightbulbNode : public SceneNode
    {
    public:
        LightbulbNode();

        void Update(double delta_time) override;

    private:
        void UpdateLightBulb(const glm::vec3 &eye, const glm::vec3 &direction);

        std::shared_ptr<PointLight> light_;

        std::shared_ptr<VertexObject> sphere_mesh_;
        std::shared_ptr<ShaderProgram> shader_;
        std::shared_ptr<Material> material_;

        double time_elapsed_;
    };

}  // namespace GLOO
