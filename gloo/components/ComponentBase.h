#pragma once

#include "ComponentType.h"

namespace GLOO {

    class SceneNode;

    class ComponentBase
    {
    public:
        virtual ~ComponentBase() = default;

        void SetNodePtr(SceneNode *node_ptr) { node_ptr_ = node_ptr; }

        SceneNode *GetNodePtr() const { return node_ptr_; }

    protected:
        SceneNode *node_ptr_;
    };

}  // namespace GLOO
