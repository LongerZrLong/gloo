#pragma once

#include "BindableBuffer.h"

#include <vector>

#include <glad/glad.h>

#include "BindGuard.h"
#include "gloo/utils.h"

namespace GLOO {

    template<class T, GLenum target>
    class VertexBuffer : public BindableBuffer
    {
    public:
        VertexBuffer(GLenum usage);
        ~VertexBuffer() override = default;

        void Update(const std::vector<T> &array);

        size_t GetSize() const { return size_; }

    private:
        size_t size_;
        GLenum usage_;
    };


    template<class T, GLenum target>
    VertexBuffer<T, target>::VertexBuffer(GLenum usage)
            : BindableBuffer(target), usage_(usage)
    {
    }

    template<class T, GLenum target>
    void VertexBuffer<T, target>::Update(const std::vector<T> &array)
    {
        BindGuard bg(this);
        GL_CHECK(glBufferData(target_, sizeof(T) * array.size(), array.data(), usage_));
        size_ = array.size();
    }

}  // namespace GLOO
