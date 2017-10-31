#pragma once

#include "Types.h"

class CScene
{
    // Set of meshes
public:
    SMesh floor, up, left, right, front;
    std::vector<SMesh> all_mesh;
    std::vector<glm::vec3> mesh_colors;
    CScene();
};

glm::vec3 HitWall(SRay ray, SMesh wall);
