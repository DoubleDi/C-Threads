#include "EasyBMP.h"

#include "Tracer.h"
#include <omp.h>

using namespace glm;

SRay CTracer::MakeRay(uvec2 pixelPos)
{
    SRay ray;
    double x = (pixelPos.x + 0.5) / xRes - 0.5;
    double y = (pixelPos.y + 0.5) / yRes - 0.5;
    ray.m_dir = m_camera.m_forward + glm::vec3(m_camera.m_right.x * x, m_camera.m_right.y * x, m_camera.m_right.z * x) +
    glm::vec3(m_camera.m_up.x * y, m_camera.m_up.y * y, m_camera.m_up.z * y);
	ray.m_start = m_camera.m_pos; // eye pos is fixed in 0
    return ray;
}

glm::vec3 CTracer::get_shade(glm::vec3 hit, glm::vec3 LightPos, glm::vec3 n)
{
		// standart Fong formules
	glm::vec3 eyePos = m_camera.m_pos;
	glm::vec3 p = hit;
	glm::vec3 l = glm::normalize( LightPos - p );
    glm::vec3 v = glm::normalize( eyePos   - p );
	n = glm::normalize(n);

	glm::vec3  diffColor = glm::vec3 ( 0.7, 0.7, 0.7 );
    glm::vec3 specColor = glm::vec3 ( 0.3, 0.3, 0.3 );

	const double specPower = 40;

    glm::vec3 r = -v;
	r =  glm::reflect(r, n);
    auto tmp = max(glm::dot(n, l), 0.f);
    glm::vec3 diff = glm::vec3(diffColor.x * tmp, diffColor.y * tmp, diffColor.z * tmp);
	double loc = glm::dot(l, r);
    tmp = pow(max( loc, 0.0), specPower);
    glm::vec3 spec = glm::vec3(specColor.x * tmp, specColor.y * tmp, specColor.z * tmp);

    return diff + spec;
}

vec3 CTracer::TraceRay(SRay ray)
{
    vec3 color(0, 0, 0);
    glm::vec3 hit_point = glm::vec3();
    glm::vec3 n = glm::vec3();
    int flag = 0;
    for (uint i = 0; i < m_pScene->all_mesh.size(); i++) {
        hit_point = HitWall(ray, m_pScene->all_mesh[i]);
        if (hit_point != glm::vec3()) {
            flag = 1;
            color = m_pScene->mesh_colors[i];
            if (i == 0) {
                if (hit_point.x < -500) { hit_point.x = -500 ; }
                if (hit_point.y < -500) { hit_point.y = -500; }
                if (hit_point.x > 499) { hit_point.x = 499; }
                if (hit_point.y > 499) { hit_point.y = 499; }
                auto tmp = LightMap[hit_point.x + 500][hit_point.y];
                color = glm::vec3(color.x * tmp, color.y * tmp, color.z * tmp);
            }
            auto triangle = m_pScene->all_mesh[i].m_triangles[0];
            auto vertices = m_pScene->all_mesh[i].m_vertices;
            n = glm::normalize(glm::cross(vertices[triangle.x] - vertices[triangle.y], vertices[triangle.z] - vertices[triangle.y]));
            break;
        }
    }
    if (!flag) { return glm::vec3(); }
    // get fong shade
    auto shade = get_shade(hit_point, LightPos, n);
    // add this shade
    // std::cout << shade.x << " " << shade.y << " " << shade.z << std::endl;
    color += shade;
    // translate color

    // color = glm::vec3( color.x * m_pScene -> obj[cross_boxes[i]] -> translate_color.x, color.y *m_pScene -> obj[cross_boxes[i]] -> translate_color.y, color.z * m_pScene -> obj[cross_boxes[i]] -> translate_color.z );
    return color;
}

void CTracer::RenderImage(int xres, int yres, int omp)
{
    // Rendering
    xRes = xres;
    yRes = yres;
    m_camera.m_resolution = uvec2(xRes, yRes);
    m_camera.m_pixels.resize(xRes * yRes);
    LightPos = glm::vec3(0, 200, 200);
    LightDir = glm::normalize(glm::vec3(0, 0, -1));

    m_camera.m_pos = glm::vec3(0, 0, 0);
    m_camera.m_forward = glm::vec3(0, 200, 0);
    m_camera.m_right = glm::vec3(1000, 0, 0);
    m_camera.m_up = glm::vec3(0, 0, 1000);
    LightMap.resize(1000);
    for (int i = 0; i < 1000; i++) {
        LightMap[i].resize(1000);
    }
    if (omp) {
        #pragma omp parallel
        {
            int i, j;
            #pragma omp for private(j)
            for (i = 0; i < 1000; i++) {
                for (j = 0; j < 1000; j++) {
                    LightMap[i][j] = 1;
                }
            }

            #pragma omp barrier

            #pragma omp for
            for (i = 0; i < 1000; i++) {
                SRay ray;
                ray.m_start = LightPos;
                ray.m_dir = LightDir + glm::vec3(rand() / RAND_MAX - 0.5 , rand() / RAND_MAX - 0.5 , rand() / RAND_MAX - 0.5);
                glm::vec3 hit = HitWall(ray, m_pScene->floor);
                if (hit != glm::vec3()) {
                    LightMap[hit.x + 500 > 999 ? 999 : hit.y + 500][hit.y + 500 > 999 ? 999 : hit.y + 500] = 3;
                }
            }

            #pragma omp barrier

            #pragma omp for private(j)
            for(i = 0; i < yRes; i++) {
                for(int j = 0; j < xRes; j++) {
                    SRay ray = MakeRay(uvec2(j, i));
                    m_camera.m_pixels[i * xRes + j] = TraceRay(ray);
                }
            }
        }
    } else {
        for (int i = 0; i < 1000; i++) {
            for (int j = 0; j < 1000; j++) {
                LightMap[i][j] = 1;
            }
        }
        for (int i = 0; i < 1000; i++) {
            SRay ray;
            ray.m_start = LightPos;
            ray.m_dir = LightDir + glm::vec3(rand() / RAND_MAX - 0.5 , rand() / RAND_MAX - 0.5 , rand() / RAND_MAX - 0.5);
            glm::vec3 hit = HitWall(ray, m_pScene->floor);
            if (hit != glm::vec3()) {
                LightMap[hit.x + 500 > 999 ? 999 : hit.y + 500][hit.y + 500 > 999 ? 999 : hit.y + 500] = 3;
            }
        }
        for(int i = 0; i < yRes; i++) {
            for(int j = 0; j < xRes; j++) {
                SRay ray = MakeRay(uvec2(j, i));
                m_camera.m_pixels[i * xRes + j] = TraceRay(ray);
            }
        }
    }
}

void CTracer::SaveImageToFile(std::string fileName)
{
    BMP image;

    int width = m_camera.m_resolution.x;
    int height = m_camera.m_resolution.y;

    image.SetSize(width, height);

    RGBApixel p;

    int textureDisplacement = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            vec3 color = m_camera.m_pixels[textureDisplacement + j];

            p.Red   = clamp(color.r, 0.0f, 1.0f) * 255.0f;
            p.Green = clamp(color.g, 0.0f, 1.0f) * 255.0f;
            p.Blue  = clamp(color.b, 0.0f, 1.0f) * 255.0f;

            image.SetPixel(j, i, p);
        }

        textureDisplacement += width;
    }

    image.WriteToFile(fileName.c_str());
}

/*
BMP *CTracer::LoadImageFromFile(std::string fileName)
{
    BMP pImage;

    if (!pImage.ReadFromFile(fileName.c_str())) {
        return NULL;
    } else {
        return pImage;
    }
}
*/
