#pragma once

#include "gloo/utils.h"
#include "gloo/alias_types.h"

namespace GLOO {
    std::unique_ptr<NormalArray> CalculateNormals(const PositionArray &positions, const IndexArray &indices);
}
