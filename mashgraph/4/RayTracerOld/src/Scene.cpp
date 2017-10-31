#include "Scene.h"
#include <algorithm>
using namespace std;

#define PI 3.1415
#define EPSILON 0.000001 // for comparing double with zero
float all_scale = 1.0 ; // scale and translate all objects of scene


Mesh * p_mesh;



object::object() { blink=true; }

object::object(object*o){}


	// building scene
	// l3ds loader from 3 task
CScene::CScene()
{
		// varibles
	L3DS scene;
	object * o;

		// loading 2-polygonal model ( more 2 - easy "for"-c )
	if (!scene.LoadFile("highpolygonal/table.3ds")) { cout<<" bad .3ds file \r\n"; return;}
	//cout<<scene.GetMeshCount(); // number of polygones
	LMesh &mesh = scene.GetMesh(0);
	LMesh &mesh1 = scene.GetMesh(1);
	cout<<mesh.GetTriangleCount()<<"+"<<mesh1.GetTriangleCount()<<" triangles in 3ds. \r\n";

	p_mesh = new Mesh();

	p_mesh->dataCount = mesh.GetTriangleCount()*3+mesh1.GetTriangleCount()*3;
	unsigned int nTriangles = mesh.GetTriangleCount()+mesh1.GetTriangleCount();
	p_mesh->indicesCount = (mesh.GetTriangleCount()+mesh1.GetTriangleCount())*3;
	p_mesh->pData = new VertexData [p_mesh->dataCount];
	p_mesh->pIndices = new unsigned int [p_mesh->indicesCount];

		// scale unuse!!!
	float x_scale = 1;
	float y_scale = 1;
	float z_scale = 1;
};

bool object::check3( SRay ray, double &t, VertexData * ug_1, VertexData * ug_2, VertexData * ug_3, glm::vec3 & n)
{
	auto copy_norm_ray = ray.m_dir.length();
		// translating matrix, rays to local coordinates
	glm::mat4 a = glm::mat4(1.0);
	a.makeInverseTransform(glm::vec3(), scale_mesh,  Common::Quaternion(rotate, axis));
	ray.m_dir = a * ray.m_dir;
	a = glm::mat4(1.0);
	a.makeInverseTransform(translate_mesh, scale_mesh,  Common::Quaternion(rotate, axis));
	ray.m_start = a*ray.m_start;

			// find nearest hitting
		// start varible eq
	float t_min = -1;
	int num_min=-1;
	double u_m, v_m, u, v;
	glm::vec3 d;
		// across our mesh
		// algorithm as in ray-tracing.ru , but very optimized, as at wiki.en, but bit changed and added own functions
	for ( int i = 0; i <= own_mesh -> dataCount; i+=3 )
	{
		auto e1 = own_mesh -> pData[i+1].pos - own_mesh -> pData[i].pos;
		auto e2 = own_mesh -> pData[i+2].pos - own_mesh -> pData[i].pos;
		auto T = ray.m_start - own_mesh -> pData[i].pos;
		auto p = glm::cross(ray.m_dir, e2);
		d = ray.m_dir;

		double det = glm::dot(p, e1);
		if( det > -EPSILON && det < EPSILON ) continue;
		double inv_det = 1.0 / det;

		u = inv_det*T.dotProduct(p);
		if( u < 0.f || u > 1.f ) continue;

		auto q = T.crossProduct( e1 );
		v = inv_det * d.dotProduct( q );
		if( v < 0.f || u + v  > 1.f ) continue;

		t = inv_det * e2.dotProduct( q );

			if ( ( t < t_min && t > EPSILON || num_min == -1 ) && ( t == t ) )
			{
				t_min = t;
				u_m = u;
				v_m = v;
				num_min = i;
			}
	}
		// nearest hitting
	t = t_min;
		// there is hit
	if ( num_min != -1 )
	{
		a = glm::mat4(1.0);
		a.makeTransform( translate_mesh, scale_mesh,  Common::Quaternion( rotate, axis ) );

			// angles point of hitting triangle
		ug_1 -> pos = a * ( own_mesh -> pData[num_min].pos );
		ug_2 -> pos = a * ( own_mesh -> pData[num_min+1].pos );
		ug_3 -> pos = a * ( own_mesh -> pData[num_min+2].pos );

		a=glm::transpose(glm::inverse(a));

			// angles normales of hitting triangle
		ug_1 -> nor = a * ( own_mesh -> pData[num_min].nor );
		ug_2 -> nor = a * ( own_mesh -> pData[num_min+1].nor );
		ug_3 -> nor = a * ( own_mesh -> pData[num_min+2].nor );

			// smooth normales
		n = u_m * ug_2->nor + v_m * ug_3->nor + ( 1 - u_m - v_m ) * ug_1 -> nor;
			// there is hit
		return true;
	}
		// no hit
	return false;
}
