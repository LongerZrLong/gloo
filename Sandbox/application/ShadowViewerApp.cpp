#include "ShadowViewerApp.h"

#include <gloo/shaders/PhongShader.h>
#include <gloo/components/ShadingComponent.h>
#include <gloo/components/MaterialComponent.h>
#include <gloo/MeshLoader.h>
#include <gloo/lights/AmbientLight.h>
#include <gloo/cameras/ArcBallCameraNode.h>

#include "SunNode.h"

namespace {

    void SetAmbientToDiffuse(GLOO::MeshData &mesh_data)
    {
        // Certain groups do not have an ambient color, so we use their diffuse colors
        // instead.
        for (auto &g : mesh_data.groups)
        {
            if (glm::length(g.material->GetAmbientColor()) < 1e-3)
            {
                g.material->SetAmbientColor(g.material->GetDiffuseColor());
                g.material->SetAmbientTexture(g.material->GetDiffuseTexture());
            }
        }
    }
}  // namespace

namespace GLOO {

    ShadowViewerApp::ShadowViewerApp(const std::string &app_name,
                                     glm::ivec2 window_size)
            : Application(app_name, window_size)
    {
    }

    void ShadowViewerApp::SetupScene()
    {
        SceneNode &root = scene_->GetRootNode();

        // Setting up the camera. PLEASE DO NOT MODIFY THE INITIAL CAMERA TRANSFORM.
        auto camera_node = make_unique<ArcBallCameraNode>(50.0f, 1.0f, 10.0f);
        camera_node->GetTransform().SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
        camera_node->GetTransform().SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), kPi / 2);
        camera_node->Calibrate();
        scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
        root.AddChild(std::move(camera_node));

        // Add in the ambient light so the shadowed areas won't be completely black.
        auto ambient_light = std::make_shared<AmbientLight>();
        ambient_light->SetAmbientColor(glm::vec3(0.15f));
        auto ambient_node = make_unique<SceneNode>();
        ambient_node->CreateComponent<LightComponent>(ambient_light);
        root.AddChild(std::move(ambient_node));

        // Add in the directional light as the sun.
        root.AddChild(make_unique<SunNode>());
        // root.AddChild(make_unique<LightbulbNode>());

        // Load and set up the scene OBJ.
        MeshData mesh_data = MeshLoader::Import("Sandbox/sponza_low/sponza_norm.obj");
        SetAmbientToDiffuse(mesh_data);

        std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
        std::shared_ptr<VertexObject> vertex_obj = std::move(mesh_data.vertex_obj);

        // TODO: Create the SceneNodes. All objects in the scene will share a
        // VertexObject. Each object is represented as a "group" within
        // mesh_data.groups. You will need to create a SceneNode for each object and
        // create a ShadingComponent and a RenderingComponent for it before adding the
        // node to the scene tree. You will also need to set the draw range of the
        // RenderingComponent to start at start_face_index of each group, with the
        // number of indices equal to num_indices of the group.

        for (auto &mesh_group : mesh_data.groups)
        {
            auto scene_ptr = make_unique<SceneNode>();
            scene_ptr->CreateComponent<ShadingComponent>(shader);
            scene_ptr->CreateComponent<MaterialComponent>(mesh_group.material);
            auto &rc = scene_ptr->CreateComponent<RenderingComponent>(vertex_obj);
            rc.SetDrawRange(mesh_group.start_face_index, mesh_group.num_indices);
            root.AddChild(std::move(scene_ptr));
        }
    }

}  // namespace GLOO
