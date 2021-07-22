#pragma once

#include <typeinfo>

#include "gloo/utils.h"

namespace GLOO {

    enum class ComponentType
    {
        Undefined = 0,
        Rendering,
        Shading,
        Material,
        Camera,
        Light,
        Tracing,
    };

    template<typename T>
    struct ComponentTrait
    {
        static ComponentType GetType()
        {
            static_assert(always_false<T>::value, "Unimplemented component type!");
            return ComponentType::Undefined;
        }
    };

#define CREATE_COMPONENT_TRAIT(C, T) \
  template <>                        \
  struct ComponentTrait<C> {         \
    static ComponentType GetType() { \
      return T;                      \
    }                                \
  };

}  // namespace GLOO
