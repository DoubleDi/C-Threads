#include "EasyBMP.h"

#include "Tracer.h"

using namespace glm;

int distance_to_image = 150;

void CTracer::GenColorMap()
{

    // for (int i = 0; i < y; i++) {
    //     for (int j = 0; j < x; j++) {
    //         if (i <= y / 4 && j <= x / 4 && i >= j) {
    //             colormap[i][j] = glm::vec3(1, 0, 0);
    //         }
    //         else if (i <= y / 4 && j <= x / 4 && i <= j) {
    //             colormap[i][j] = glm::vec3(0, 0, 1);
    //         }
    //         else if (i >= y / 4 &&  i <= 3 * y / 4 && j <= x / 4) {
    //             colormap[i][j] = glm::vec3(1, 0, 0);
    //         }
    //         else if (i >= 3 * y / 4 && j <= x / 4 && y - i >= j) {
    //             colormap[i][j] = glm::vec3(1, 0, 0);
    //         }
    //         else if (i >= 3 * y / 4 && j <= x / 4 && y - i <= j) {
    //             colormap[i][j] = glm::vec3(0, 0, 1);
    //         }
    //         else if (i <= y / 4 && j >= x / 4 && j <= 3 * x / 4) {
    //             colormap[i][j] = glm::vec3(0, 0, 1);
    //         }
    //         else if (i >= y / 4 && i <= 3 * y / 4 && j >= x / 4 && j <= 3 * x / 4) {
    //             colormap[i][j] = glm::vec3(1, 1, 1);
    //         }
    //         else if (i >= 3 * y / 4 && j >= x / 4  && j <= 3 * x / 4) {
    //             colormap[i][j] = glm::vec3(0, 0, 1);
    //         }
    //         else if (i <= y / 4 && j >= 3 * x / 4 && i <= x - j) {
    //             colormap[i][j] = glm::vec3(0, 0, 1);
    //         }
    //         else if (i <= y / 4 && j >= 3 * x / 4 && i >= x - j) {
    //             colormap[i][j] = glm::vec3(0, 1, 0);
    //         }
    //         else if (i >= y / 4 && i <= 3 * y / 4 && j >= 3 * x / 4) {
    //             colormap[i][j] = glm::vec3(0, 1, 0);
    //         }
    //         else if (i >= 3 * y / 4 && j >= 3 * x / 4 && i <= j) {
    //             colormap[i][j] = glm::vec3(0, 1, 0);
    //         }
    //         else if (i >= 3 * y / 4 && j >= 3 * x / 4 && i >= j) {
    //             colormap[i][j] = glm::vec3(0, 0, 1);
    //         }
    //     }
    // }
}

SRay CTracer::MakeRay(uvec2 pixelPos)
{
    SRay ray = SRay();
	ray.m_dir = glm::normalize(glm::vec3(1.0*pixelPos.x-1.0*xres/2, 1.0*pixelPos.y-1.0*yres/2, distance_to_image));
	ray.m_start = glm::vec3();
	return ray;
}

bool RayBoxIntersection( glm::vec3 ray_pos, glm::vec3 inv_dir, glm::vec3 boxMin, glm::vec3 boxMax, float& tmin, float& tmax )
{
  inv_dir = glm::vec3(1 / inv_dir.x, 1 / inv_dir.y, 1 / inv_dir.z);
  float lo = inv_dir.x * ( boxMin.x - ray_pos.x );
  float hi = inv_dir.x * ( boxMax.x - ray_pos.x );
  tmin  = min( lo, hi );
  tmax = max( lo, hi );

  float lo1 = inv_dir.y * ( boxMin.y - ray_pos.y );
  float hi1 = inv_dir.y * ( boxMax.y - ray_pos.y );
  tmin = max( tmin, min(lo1, hi1) );
  tmax = min( tmax, max(lo1, hi1) );

  float lo2 = inv_dir.z * ( boxMin.z - ray_pos.z );
  float hi2 = inv_dir.z * ( boxMax.z - ray_pos.z );
  tmin = max( tmin, min( lo2, hi2 ) );
  tmax = min( tmax, max( lo2, hi2 ) );

  return ( tmin <= tmax ) && ( tmax > 0.f );
}

// Get base color ( Fong )
glm::vec3 CTracer::get_shade( glm::vec3 hit, glm::vec3 lightPos, glm::vec3 n, bool blink )
{
        // standart Fong formules
    glm::vec3 eyePos = glm::vec3(0,0,0);
    glm::vec3 p = hit;
    glm::vec3 l =  glm::normalize( lightPos - p );
    glm::vec3 v = glm::normalize( eyePos   - p );
    n = glm::normalize(n);

    glm::vec3  diffColor = glm::vec3 ( 0.7, 0.7, 0.7 );
    glm::vec3 specColor = glm::vec3 ( 0.3 * blink, 0.3 * blink, 0.3 * blink); // no blinking material

    const double specPower = 40;

    glm::vec3 r = -v;
        r =  glm::reflect(r, n);
    auto tmp = std::max ( glm::dot(n, l), 0.f );
    glm::vec3 diff = glm::vec3(diffColor.x * tmp, diffColor.y * tmp, diffColor.z * tmp);
    double loc = glm::dot(l, r);
    tmp = pow (max( loc, 0.0 ), specPower);
    glm::vec3 spec = glm::vec3(specColor.x * tmp, specColor.y * tmp, specColor.z * tmp);

    return diff + spec;
}


std::vector<int> cross_boxes( SRay ray, CScene * m_pScene )
{
	std::vector <int> result; // array of number of boxes
	std::vector <float> min_mas; // array of minimum  distance to boxes
	for (uint i = 0; i < m_pScene -> obj.size(); i++ )
	{
		float min, max; // for using our function. max - not use
		if ( RayBoxIntersection ( ray.m_start, ray.m_dir, m_pScene -> obj[i] -> box.getMinimum(), m_pScene -> obj[i] -> box.getMaximum(), min, max ) == true )
		{
				// if crossing
			result.push_back( i );
			min_mas.push_back( min );
		}
	}
		//sorting our array.
		// it's very small arrays => easy sorting
	 for ( uint i = 0; i < result.size(); i++ )
       for ( uint j = i + 1; j < result.size(); j++ )
          if ( min_mas[i] > min_mas[j] )
          {
				// swap
             auto buf = min_mas[i];
             min_mas[i] = min_mas[j];
             min_mas[j] = buf;
				// swap
			 auto buf1 = result[i];
             result[i] = result[j];
             result[j] = buf1;
          }
	return result;
}
SRay CTracer::HelpingRay(glm::vec3 x, glm::vec3 y) // Create all ray
{
	auto ray = SRay();
	ray.m_dir = y - x; // easy
	ray.m_start = x;
	return ray;
}

bool CTracer::visible( glm::vec3 x, glm::vec3 y, int num )
{
	SRay ray = HelpingRay( y, x ); // ray from light
	std::vector < int > cross_b = cross_boxes( ray, m_pScene ); // array crossing boxes
	if ( cross_b.size() == 0 ) return true; // no crossing boxes
		// else branch
	double t; // distance to hit
	VertexData* tmp = new VertexData; // temp variable for using our func without changes
		uint i = 0;
		// check hiting in every box
	// while ( ( i < cross_b.size() ) && ( ( m_pScene -> obj[cross_b[i]] -> shadow == false ) || !( m_pScene -> obj[cross_b[i]] -> check3( ray, t, tmp, tmp, tmp, glm::vec3() ) ) ) ) i++;
		// free memory
	delete tmp;
		// no hit
	if ( i >= cross_b.size() ) return true;
		// bad hit
	if ( cross_b[i] == num ) return true;
	return false;
}

vec3 CTracer::TraceRay(SRay ray)
{
    glm::vec3 color = glm::vec3();

    std::vector<int> cross_b = cross_boxes( ray, m_pScene );
    if ( cross_b.size() == 0 ) { return color; }

    double t; // distance to hit
    uint i = 0;
    // sides of triangle
    VertexData * ug_1 = new VertexData;
    VertexData * ug_2 = new VertexData;
    VertexData * ug_3 = new VertexData;
    glm::vec3 n;

    // check hit in every box
    while ( ( i < cross_b.size() ) && !( m_pScene -> obj[cross_b[i]] -> check3( ray, t, ug_1, ug_2, ug_3, n) ) ) i++;
    // free memory
    delete ug_1;
    delete ug_2;
    delete ug_3;
    // no hit
    if ( i >= cross_b.size() ) { return color; }
    // else branch

    // compute point of hit
    auto hit_point = ray.m_start + glm::vec3(ray.m_dir.x*t,ray.m_dir.y*t,ray.m_dir.z*t) ;
    // position of light
    // it's behid eye
    // we don't see it

    // check for shadow
    if ( visible( hit_point, LightPos, cross_b[i] ) )
    {
        // get fong shade
    auto shade = get_shade ( hit_point, LightPos, n, m_pScene -> obj[cross_b[i]] -> blink );
        // add this shade
    color += vec3(shade.x,shade.y,shade.z);
        // translate color
    color = glm::vec3( color.x * m_pScene -> obj[cross_b[i]] -> translate_color.x, color.y *m_pScene -> obj[cross_b[i]] -> translate_color.y, color.z * m_pScene -> obj[cross_b[i]] -> translate_color.z );
}

    // reflecting ray
    return color;
}


void CTracer::RenderImage(int xRes, int yRes)
{
    // Rendering
    m_camera.m_resolution = uvec2(xRes, yRes);
    m_camera.m_pixels.resize(xRes * yRes);
    distance_to_image=distance_to_image*yRes/768;
    // GenColorMap();
    xres = xRes;
    yres = yRes;

    for(int i = 0; i < yRes; i++) {
        for(int j = 0; j < xRes; j++) {
            SRay ray = MakeRay(uvec2(j, i));
            m_camera.m_pixels[i * xRes + j] = TraceRay(ray);
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
