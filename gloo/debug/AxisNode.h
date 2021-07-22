#include "gloo/SceneNode.h"

namespace GLOO {

    class AxisNode : public SceneNode
    {
    public:
        AxisNode(int key);

        void Update(double delta_time) override;

    private:
        void ToggleAxes();
        void PlotAxes();

    private:
        int key_;
        SceneNode *x_line_node_ptr_;
        SceneNode *y_line_node_ptr_;
        SceneNode *z_line_node_ptr_;
    };

}  // namespace GLOO
