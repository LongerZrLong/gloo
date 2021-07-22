#pragma once

#include "parsers/ObjParser.h"
#include "MeshData.h"

namespace GLOO {

    class MeshLoader
    {
    public:
        static MeshData Import(const std::string &filename);
    };

}  // namespace GLOO
