#pragma once

#include <glad/glad.h>

#include "IBindable.h"

namespace GLOO {

    class BindableBuffer : public IBindable
    {
    public:
        BindableBuffer(GLenum target);
        ~BindableBuffer() override;

        BindableBuffer(const BindableBuffer &) = delete;
        BindableBuffer &operator=(const BindableBuffer &) = delete;

        // Allow both move-construct and move-assign.
        BindableBuffer(BindableBuffer &&other) noexcept;
        BindableBuffer &operator=(BindableBuffer &&other) noexcept;

        void Reset(GLuint handle = 0);

        GLuint Release();

        void Bind() const override;
        void Unbind() const override;

    private:
        GLuint handle_;

    protected:
        GLenum target_;
    };
}  // namespace GLOO
