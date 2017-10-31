#include "Scene.h"


CScene::CScene()
{
    floor.m_vertices = {
        glm::vec3(-500, 0, -500),
        glm::vec3(-500, 1000, -500),
        glm::vec3(500, 1000, -500),
        glm::vec3(500, 0, -500),
    };
    floor.m_triangles = {
        glm::vec3(0, 1, 2),
        glm::vec3(0, 3, 2)
    };
    up.m_vertices = {
        glm::vec3(-500, 0, 500),
        glm::vec3(-500, 1000, 500),
        glm::vec3(500, 1000, 500),
        glm::vec3(500, 0, 500),
    };
    up.m_triangles = {
        glm::vec3(0, 1, 2),
        glm::vec3(0, 3, 2),
    };
    left.m_vertices = {
        glm::vec3(-500, 0, -500),
        glm::vec3(-500, 1000, -500),
        glm::vec3(-500, 1000, 500),
        glm::vec3(-500, 0, 500),
    };
    left.m_triangles = {
        glm::vec3(0, 1, 2),
        glm::vec3(0, 3, 2),
    };
    right.m_vertices = {
        glm::vec3(500, 0, -500),
        glm::vec3(500, 1000, -500),
        glm::vec3(500, 1000, 500),
        glm::vec3(500, 0, 500),
    };
    right.m_triangles = {
        glm::vec3(0, 1, 2),
        glm::vec3(0, 3, 2),
    };
    front.m_vertices = {
        glm::vec3(-500, 1000, -500),
        glm::vec3(-500, 1000, 500),
        glm::vec3(500, 1000, 500),
        glm::vec3(500, 1000, -500),
    };
    front.m_triangles = {
        glm::vec3(0, 1, 2),
        glm::vec3(0, 3, 2),
    };
    all_mesh = {
        floor,
        up,
        front,
        left,
        right
    };
    mesh_colors = {
        glm::vec3(0,0,0.5),
        glm::vec3(0,0.5,0),
        glm::vec3(0.5,0.5,0.5),
        glm::vec3(0.5,0,0),
        glm::vec3(0.5,0,0)
    };
}

double Area(glm::vec3 a, glm::vec3 b)
{
    auto vec = glm::cross(a, b);
    return 0.5*sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

bool IsInside(glm::vec3 dot, SMesh wall)
{
    for (auto tr : wall.m_triangles) {
        auto dot1 = wall.m_vertices[tr.x];
        auto dot2 = wall.m_vertices[tr.y];
        auto dot3 = wall.m_vertices[tr.z];
        auto area = Area(dot1 - dot2, dot3 - dot2);
        auto area1 = Area(dot - dot1, dot - dot2);
        auto area2 = Area(dot - dot2, dot - dot3);
        auto area3 = Area(dot - dot3, dot - dot1);
        if (area - area1 - area2 - area3 < 0.01) {
            return true;
        }
    }
    return false;
}

glm::vec3 HitWall(SRay ray, SMesh wall)
{
    auto a = wall.m_vertices[0], b = wall.m_vertices[1], c = wall.m_vertices[2];
    auto dot = a;
    auto normal = glm::normalize(glm::cross(c - a, b - a));
    double t = - glm::dot(ray.m_start - dot, normal) / glm::dot(ray.m_dir, normal);
    auto res = ray.m_start + glm::vec3(t * ray.m_dir.x, t * ray.m_dir.y, t * ray.m_dir.z);
    if (IsInside(res, wall)) {
        return res;
    } else {
        return glm::vec3();
    }
}
