#pragma once

#include <gloo/Application.h>

namespace GLOO {

    class ShadowViewerApp : public Application
    {
    public:
        ShadowViewerApp(const std::string &app_name, glm::ivec2 window_size);

        void SetupScene() override;
    };

}  // namespace GLOO
