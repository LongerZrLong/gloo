#pragma once

#include "BindGuard.h"
#include "gloo/external.h"
#include "Texture.h"

namespace GLOO {

    class Framebuffer : public IBindable
    {
    public:
        Framebuffer();
        ~Framebuffer() override;

        Framebuffer(const Framebuffer &) = delete;
        Framebuffer &operator=(const Framebuffer &) = delete;

        // Allow both move-construct and move-assign.
        Framebuffer(Framebuffer &&other) noexcept;

        Framebuffer &operator=(Framebuffer &&other) noexcept;

        void Bind() const override;
        void Unbind() const override;

        void AssociateTexture(const Texture &texture, GLenum attachment);

    private:
        GLuint handle_{GLuint(-1)};
    };

}  // namespace GLOO
