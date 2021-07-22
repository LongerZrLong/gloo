#pragma once

#include "ShaderProgram.h"

namespace GLOO {

    class ShadowShader : public ShaderProgram
    {
    public:
        ShadowShader();

        void SetTargetNode(const SceneNode &node, const glm::mat4 &model_matrix) const override;

        void SetW2LNDCMatrix(const glm::mat4 &world_to_light_ndc_matrix) const;

    private:
        void AssociateVertexArray(VertexArray &vertex_array) const;
    };
}