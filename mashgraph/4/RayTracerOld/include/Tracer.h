#pragma once

#include "glm.hpp"
#include "EasyBMP.h"

#include "Types.h"
#include "Scene.h"

#include "string"
// using namespace std;

class CTracer
{
public:
    SRay MakeRay(glm::uvec2 pixelPos);  // Create ray for specified pixel
    glm::vec3 TraceRay(SRay ray); // Trace ray, compute its color
    void RenderImage(int xRes, int yRes);
    void GenColorMap();
    void SaveImageToFile(std::string fileName);
    SRay HelpingRay(glm::vec3, glm::vec3);
    bool visible( glm::vec3 x, glm::vec3 y, int num );
    glm::vec3 get_shade( glm::vec3 hit, glm::vec3 lightPos, glm::vec3 n, bool blink );


public:
    glm::vec3 LightPos = glm::vec3(500, 500, 100);
    glm::vec3 LightDir = glm::vec3(0, 0.5, -1);
    SCamera m_camera;
    CScene* m_pScene;
    std::vector<std::vector<glm::vec3>> colormap;
    std::vector<std::vector<float>> lightmap;
    int xres, yres;
};
