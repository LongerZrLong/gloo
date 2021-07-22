#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtx/string_cast.hpp>

#include "Application.h"
#include "shaders/ShadowShader.h"
#include "components/ShadingComponent.h"
#include "debug/PrimitiveFactory.h"

namespace {

    const size_t kShadowWidth = 4096;
    const size_t kShadowHeight = 4096;
    const glm::mat4 kLightProjection =
            glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 80.0f);
    const glm::mat4 pLightProjection =
            glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 80.0f);

}  // namespace

namespace GLOO {

    Renderer::Renderer(Application &application) : application_(application)
    {
        UNUSED(application_);
        // TODO: you may want to initialize your framebuffer and texture(s) here.

        plain_texture_shader_ = make_unique<PlainTextureShader>();

        shadow_depth_tex_ = make_unique<Texture>();
        shadow_depth_tex_->Reserve(GL_DEPTH_COMPONENT, kShadowWidth, kShadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT);

        shadow_frame_buffer_ = make_unique<Framebuffer>();
        shadow_frame_buffer_->AssociateTexture(*shadow_depth_tex_, GL_DEPTH_ATTACHMENT);


        // To render a quad on in the lower-left of the screen, you can assign texture
        // to quad_ created below and then call quad_->GetVertexArray().Render().
        quad_ = PrimitiveFactory::CreateQuad();
    }

    void Renderer::SetRenderingOptions() const
    {
        GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

        // Enable depth test.
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glDepthFunc(GL_LEQUAL));

        // Enable blending for multi-pass forward rendering.
        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendFunc(GL_ONE, GL_ONE));
    }

    void Renderer::Render(const Scene &scene) const
    {
        SetRenderingOptions();
        RenderScene(scene);
        // TODO: When debugging your shadow map, call DebugShadowMap to render a
        // quad at the bottom left corner to display the shadow map.
        DebugShadowMap();
    }

    void Renderer::RecursiveRetrieve(const SceneNode &node,
                                     RenderingInfo &info,
                                     const glm::mat4 &model_matrix)
    {
        // model_matrix is parent to world transformation.
        glm::mat4 new_matrix =
                model_matrix * node.GetTransform().GetLocalToParentMatrix();
        auto robj_ptr = node.GetComponentPtr<RenderingComponent>();
        if (robj_ptr != nullptr && node.IsActive())
            info.emplace_back(robj_ptr, new_matrix);

        size_t child_count = node.GetChildrenCount();
        for (size_t i = 0; i < child_count; i++)
        {
            RecursiveRetrieve(node.GetChild(i), info, new_matrix);
        }
    }

    Renderer::RenderingInfo Renderer::RetrieveRenderingInfo(
            const Scene &scene) const
    {
        RenderingInfo info;
        const SceneNode &root = scene.GetRootNode();
        // Efficient implementation without redundant matrix multiplications.
        RecursiveRetrieve(root, info, glm::mat4(1.0f));
        return info;
    }

    void Renderer::RenderShadow(LightComponent *light_comp_ptr, RenderingInfo &renderingInfo) const
    {
        if (!light_comp_ptr->CanCastShadow())
        { return; }

        shadow_frame_buffer_->Bind();

        GL_CHECK(glViewport(0, 0, kShadowWidth, kShadowHeight));
        GL_CHECK(glDepthMask(GL_TRUE));
        GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
        GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

        for (const auto &pr : renderingInfo)
        {
            auto robj_ptr = pr.first;
            SceneNode &node = *robj_ptr->GetNodePtr();
            auto shader = make_unique<ShadowShader>();

            BindGuard shader_bg(shader.get());

            // Set various uniform variables in the shaders.
            shader->SetTargetNode(node, pr.second);
            shader->SetW2LNDCMatrix(kLightProjection *
                                    glm::inverse(light_comp_ptr->GetNodePtr()->GetTransform().GetLocalToWorldMatrix()));

            robj_ptr->Render();
        }

        shadow_frame_buffer_->Unbind();
        GL_CHECK(glViewport(0, 0, application_.GetWindowSize().x, application_.GetWindowSize().y));
    }

    void Renderer::RenderScene(const Scene &scene) const
    {
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        const SceneNode &root = scene.GetRootNode();
        auto rendering_info = RetrieveRenderingInfo(scene);
        auto light_ptrs = root.GetComponentPtrsInChildren<LightComponent>();
        if (light_ptrs.size() == 0)
        {
            // Make sure there are at least 2 passes of we don't forget to set color
            // mask back.
            return;
        }

        CameraComponent *camera = scene.GetActiveCameraPtr();

        {
            // Here we first do a depth pass (note that this has nothing to do with the
            // shadow map). The goal of this depth pass is to exclude pixels that are
            // not really visible from the camera, in later rendering passes. You can
            // safely leave this pass here without understanding/modifying it, for
            // assignment 5. If you are interested in learning more, see
            // https://www.khronos.org/opengl/wiki/Early_Fragment_Test#Optimization

            GL_CHECK(glDepthMask(GL_TRUE));
            bool color_mask = GL_FALSE;
            GL_CHECK(glColorMask(color_mask, color_mask, color_mask, color_mask));

            for (const auto &pr : rendering_info)
            {
                auto robj_ptr = pr.first;
                SceneNode &node = *robj_ptr->GetNodePtr();
                auto shading_ptr = node.GetComponentPtr<ShadingComponent>();
                if (shading_ptr == nullptr)
                {
                    std::cerr << "Some mesh is not attached with a shader during rendering!"
                              << std::endl;
                    continue;
                }
                ShaderProgram *shader = shading_ptr->GetShaderPtr();

                BindGuard shader_bg(shader);

                // Set various uniform variables in the shaders.
                shader->SetTargetNode(node, pr.second);
                shader->SetCamera(*camera);

                robj_ptr->Render();
            }
        }

        // The real shadow map/Phong shading passes.
        for (size_t light_id = 0; light_id < light_ptrs.size(); light_id++)
        {
            // TODO: render the shadow map viewed from the light.
            // This should be rendered to the shadow framebuffer instead of the default
            // one. You should only render shadow if the light can cast shadow (e.g.
            // directional light).

            RenderShadow(light_ptrs[light_id], rendering_info);

            GL_CHECK(glDepthMask(GL_FALSE));
            bool color_mask = GL_TRUE;
            GL_CHECK(glColorMask(color_mask, color_mask, color_mask, color_mask));

            for (const auto &pr : rendering_info)
            {
                auto robj_ptr = pr.first;
                SceneNode &node = *robj_ptr->GetNodePtr();
                auto shading_ptr = node.GetComponentPtr<ShadingComponent>();
                if (shading_ptr == nullptr)
                {
                    std::cerr << "Some mesh is not attached with a shader during rendering!"
                              << std::endl;
                    continue;
                }
                ShaderProgram *shader = shading_ptr->GetShaderPtr();

                BindGuard shader_bg(shader);

                // Set various uniform variables in the shaders.
                shader->SetTargetNode(node, pr.second);
                shader->SetCamera(*camera);

                LightComponent &light = *light_ptrs.at(light_id);
                shader->SetLightSource(light);
                // TODO: pass in the shadow texture to the shader via SetShadowMapping if
                // the light can cast shadow.

                if (light_ptrs[light_id]->CanCastShadow())
                {
                    shader->SetShadowMapping(*shadow_depth_tex_,
                                             kLightProjection * glm::inverse(
                                                     light_ptrs[light_id]->GetNodePtr()->GetTransform().GetLocalToWorldMatrix()));
                }

                robj_ptr->Render();
            }
        }

        // Re-enable writing to depth buffer.
        GL_CHECK(glDepthMask(GL_TRUE));
    }

    void Renderer::RenderTexturedQuad(const Texture &texture, bool is_depth) const
    {
        BindGuard shader_bg(plain_texture_shader_.get());
        plain_texture_shader_->SetVertexObject(*quad_);
        plain_texture_shader_->SetTexture(texture, is_depth);
        quad_->GetVertexArray().Render();
    }

    void Renderer::DebugShadowMap() const
    {
        GL_CHECK(glDisable(GL_DEPTH_TEST));
        GL_CHECK(glDisable(GL_BLEND));

        glm::ivec2 window_size = application_.GetWindowSize();
        glViewport(0, 0, window_size.x / 4, window_size.y / 4);
        RenderTexturedQuad(*shadow_depth_tex_, true);

        glViewport(0, 0, window_size.x, window_size.y);
    }

}  // namespace GLOO