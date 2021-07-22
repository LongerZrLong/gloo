#pragma once

#include "ComponentBase.h"

#include "gloo/VertexObject.h"

namespace GLOO {

    class RenderingComponent : public ComponentBase
    {
    public:
        RenderingComponent(std::shared_ptr<VertexObject> vertex_obj);

        void SetDrawRange(int start_index, int num_indices);
        void SetVertexObject(std::shared_ptr<VertexObject> vertex_obj);
        void SetDrawMode(DrawMode mode);
        void SetPolygonMode(PolygonMode mode);

        VertexObject *GetVertexObjectPtr() { return vertex_obj_.get(); }

        void Render() const;

    private:
        std::shared_ptr<VertexObject> vertex_obj_;
        int start_index_;
        int num_indices_;
    };

    CREATE_COMPONENT_TRAIT(RenderingComponent, ComponentType::Rendering);

}  // namespace GLOO
