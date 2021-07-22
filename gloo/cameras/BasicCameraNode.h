#pragma once

#include "gloo/SceneNode.h"

namespace GLOO {

    class BasicCameraNode : public SceneNode
    {
    public:
        BasicCameraNode(float fov = 45.f, float aspect = 0.75f, float speed = 2.0f);

        void Update(double delta_time) override;

    private:
        void UpdateViewport();

    private:
        float speed_;
    };

}  // namespace GLOO
