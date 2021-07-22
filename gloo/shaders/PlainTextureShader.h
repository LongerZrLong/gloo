#pragma once

#include "ShaderProgram.h"

#include "gloo/gl_wrapper/Texture.h"
#include "gloo/VertexObject.h"

namespace GLOO {

    class PlainTextureShader : public ShaderProgram
    {
    public:
        PlainTextureShader();

        void SetVertexObject(const VertexObject &obj) const;
        void SetTexture(const Texture &texture, bool is_depth) const;

    private:
        void AssociateVertexArray(const VertexArray &vertex_array) const;
    };

}  // namespace GLOO
