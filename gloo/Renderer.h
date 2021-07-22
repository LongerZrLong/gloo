#pragma once

#include <unordered_map>

#include "components/LightComponent.h"
#include "components/RenderingComponent.h"
#include "gl_wrapper/Texture.h"
#include "gl_wrapper/Framebuffer.h"
#include "shaders/PlainTextureShader.h"

namespace GLOO {

    class Scene;
    class Application;

    class Renderer
    {
    public:
        Renderer(Application &application);

        void Render(const Scene &scene) const;

    private:
        using RenderingInfo = std::vector<std::pair<RenderingComponent *, glm::mat4>>;

        void RenderScene(const Scene &scene) const;
        void RenderShadow(LightComponent *light_comp_ptr, RenderingInfo &renderingInfo) const;

        void SetRenderingOptions() const;

        RenderingInfo RetrieveRenderingInfo(const Scene &scene) const;

        static void RecursiveRetrieve(const SceneNode &node,
                                      RenderingInfo &info,
                                      const glm::mat4 &model_matrix);

        std::unique_ptr<VertexObject> quad_;

        void RenderTexturedQuad(const Texture &texture, bool is_depth) const;

        void DebugShadowMap() const;

        std::unique_ptr<Framebuffer> shadow_frame_buffer_;
        std::unique_ptr<Texture> shadow_depth_tex_;
        std::unique_ptr<PlainTextureShader> plain_texture_shader_;
        Application &application_;
    };

}  // namespace GLOO
