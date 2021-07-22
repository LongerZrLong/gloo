#pragma once

#include <gloo/SceneNode.h>

#include <gloo/lights/DirectionalLight.h>

namespace GLOO {

    class SunNode : public SceneNode
    {
    public:
        SunNode();

        void Update(double delta_time) override;

    private:
        void UpdateSun(const glm::vec3 &eye, const glm::vec3 &direction);

    private:
        std::shared_ptr<DirectionalLight> light_;
        double time_elapsed_;
    };

}  // namespace GLOO
