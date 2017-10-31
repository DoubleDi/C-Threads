#pragma once

#include "Types.h"
#include "l3ds.h"
// from 3 task shader


struct VertexData
{
    glm::vec3 pos;
    glm::vec3 nor;
};
// from 3 task MyObject
class AxisAlignedBox
{
public:
    enum Extent
    {
        EXTENT_NULL,
        EXTENT_FINITE,
        EXTENT_INFINITE
    };
public:

    glm::vec3 mMinimum;
    glm::vec3 mMaximum;
    Extent mExtent;
    glm::vec3* mCorners;

    glm::vec3& getMinimum(void)
    {
        return mMinimum;
    }

    glm::vec3& getMaximum(void)
    {
        return mMinimum;
    }

};

struct Mesh
{
    VertexData* pData;	//pointer to object's internal data
    unsigned int dataCount;
    unsigned int* pIndices;	//pointer to indexes (list of vetrices)
    unsigned int indicesCount;
    AxisAlignedBox box;
};

struct object
{
    Mesh * own_mesh;
        // for instancing
    glm::vec3 translate_mesh;
    glm::vec3 scale_mesh;
    double rotate;
    glm::vec3 axis;
        // round box
    AxisAlignedBox box;
        // material
    float reflection;
    bool shadow;
    bool blink;
        // translate rgb
        // color of material
    glm::vec3 translate_color;
        // checking for hit
    bool check3( SRay ray, double &t, VertexData * ug_1, VertexData * ug_2, VertexData * ug_3, glm::vec3 & n );

    object(object * o);
    object();
};

struct CScene
{
    // Set of objects
    std::vector <object *> obj;
    CScene();
};

// public:
//     SMesh leftwall, rightwall, botwall, upwall, frontwall;
//     glm::vec3 leftwallnormal, rightwallnormal, botwallnormal,
//     upwallnormal, frontwallnormal;
//     // Set of meshes
//     CScene()
//     {
//         leftwall.m_vertices = {
//             glm::vec3(0, 0, 0),
//             glm::vec3(250, 250, 1000),
//             glm::vec3(250, 750, 1000),
//             glm::vec3(0, 0, 0),
//             glm::vec3(0, 1000, 0),
//             glm::vec3(250, 750, 1000)
//         };
//         leftwall.m_triangles = {
//             glm::uvec3(0, 1, 2),
//             glm::uvec3(3, 4, 5)
//         };
//         rightwall.m_vertices = {
//             glm::vec3(1000, 0, 0),
//             glm::vec3(750, 250, 1000),
//             glm::vec3(750, 750, 1000),
//             glm::vec3(1000, 0, 0),
//             glm::vec3(1000, 1000, 0),
//             glm::vec3(750, 750, 1000)
//         };
//         rightwall.m_triangles = {
//             glm::uvec3(0, 1, 2),
//             glm::uvec3(3, 4, 5)
//         };
//
//         upwall.m_vertices = {
//             glm::vec3(0, 1000, 0),
//             glm::vec3(250, 750, 1000),
//             glm::vec3(750, 750, 1000),
//             glm::vec3(0, 1000, 0),
//             glm::vec3(1000, 1000, 0),
//             glm::vec3(750, 750, 1000),
//         };
//         upwall.m_triangles = {
//             glm::uvec3(0, 1, 2),
//             glm::uvec3(3, 4, 5)
//         };
//
//         frontwall.m_vertices = {
//             glm::vec3(250, 250, 1000),
//             glm::vec3(250, 750, 1000),
//             glm::vec3(750, 750, 1000),
//             glm::vec3(250, 250, 1000),
//             glm::vec3(750, 250, 1000),
//             glm::vec3(750, 750, 1000)
//         };
//         frontwall.m_triangles = {
//             glm::uvec3(0, 1, 2),
//             glm::uvec3(3, 4, 5)
//         };
//
//         botwall.m_vertices = {
//             glm::vec3(0, 0, 0),
//             glm::vec3(250, 250, 1000),
//             glm::vec3(750, 250, 1000),
//             glm::vec3(0, 0, 0),
//             glm::vec3(1000, 0, 0),
//             glm::vec3(750, 250, 1000)
//         };
//         botwall.m_triangles = {
//             glm::uvec3(0, 1, 2),
//             glm::uvec3(3, 4, 5)
//         };
//     }
// };
// //
