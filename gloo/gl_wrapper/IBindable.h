#pragma once

namespace GLOO {

    class IBindable
    {
    public:
        virtual ~IBindable() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
    };

}  // namespace GLOO
