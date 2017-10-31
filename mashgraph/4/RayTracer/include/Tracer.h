#pragma once

#include "glm.hpp"
#include "EasyBMP.h"

#include "Types.h"
#include "Scene.h"

#include "string"

class CTracer
{
public:
    SRay MakeRay(glm::uvec2 pixelPos);  // Create ray for specified pixel
    glm::vec3 TraceRay(SRay ray); // Trace ray, compute its color
    void RenderImage(int xRes, int yRes, int omp);
    void SaveImageToFile(std::string fileName);
    glm::vec3 get_shade(glm::vec3 hit, glm::vec3 LightPos, glm::vec3 n);
    // BMP LoadImageFromFile(std::string fileName);

public:
    SCamera m_camera;
    CScene* m_pScene;
    glm::vec3 LightPos;
    glm::vec3 LightDir;
    std::vector<std::vector<double>> LightMap;
    int xRes, yRes;
};
