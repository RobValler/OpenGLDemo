/*
 * WorldMovement.cpp
 *
 *  Created on: Mar 19, 2013
 *      Author: robert
 */


#include <string>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>


#include "../ObjectFactory/IObjectFactory.h"
#include "../ObjectFactory/IObjectFactoryClient.h"

#include "../Models/ModelDataStruct.h"

#include "../Tools/debug_output.h"

#include "WorldMovement.h"

#define OBJECT_HEIGHT 3.5f
#define terrain_collision_model 1

typedef struct _fPointStruct {

	float x;
	float z;

} fPointStruct;

typedef fPointStruct fPoint;


// rate of movement
#define rot_rate			1.0f
#define move_rate			0.2f
#define piover180			0.0174532925f
#define LIMIT_LOOK_UP		-90.0f
#define LIMIT_LOOK_DOWN		45.0f


static float Sign(fPoint p1, fPoint p2, fPoint p3);
static bool IsPointInTri(fPoint pt, fPoint v1, fPoint v2, fPoint v3);


WorldMovement::WorldMovement()
{


}

WorldMovement::~WorldMovement() {
	// TODO Auto-generated destructor stub
}

/*update the world, coordinates wise*/
void WorldMovement::update()
{




}

/* */

/* scan the landscape mesh to discover the triangle in contact*/
bool WorldMovement::location_ref_triangle()
{

#define LANDSCAPE_OBJECT_INDEX		0

	IObjectFactory * pIObj_Factory = (IObjectFactory*)pObjectFactory;
	triangleCoordStruct tri;

	bool result = false;
	int i;

#if 0
	float low_z, low_x;
	float high_z, high_x;
#endif

	// the landscape is typically the first object to be loaded
	IObjectFactoryClient* pObj_Factory_Entry = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryEntryPtr(LANDSCAPE_OBJECT_INDEX);

	// todo implement way to determine correct terrain vertices.
	std::vector <glm::vec3> *pVertices = (std::vector <glm::vec3> *)pObj_Factory_Entry->fetch_vertices(terrain_collision_model);


	for( i=0; i < (int)pVertices->size(); i++ )
	{
		if(( (i+1) % 3 == 0 ))
		{
			tri.x1 = pVertices->at(i-2).x;
			tri.y1 = pVertices->at(i-2).y;
			tri.z1 = pVertices->at(i-2).z;
			tri.x2 = pVertices->at(i-1).x;
			tri.y2 = pVertices->at(i-1).y;
			tri.z2 = pVertices->at(i-1).z;
			tri.x3 = pVertices->at(i).x;
			tri.y3 = pVertices->at(i).y;
			tri.z3 = pVertices->at(i).z;

#if 0
			/*position detection done by rectangle*/
			// position resolution is of a rectangle, the result acceptable but a triangle would be more accurate
			low_z = std::min(tri.z1, std::min(tri.z2, tri.z3));
			high_z = std::max(tri.z1, std::max(tri.z2, tri.z3));
			low_x = std::min(tri.x1, std::min(tri.x2, tri.x3));
			high_x = std::max(tri.x1, std::max(tri.x2, tri.x3));

			if(	(coords.k_pos_x < high_x) && (coords.k_pos_x > low_x ) &&
				(coords.k_pos_z < high_z) && (coords.k_pos_z > low_z ) )
			{
#else
			/*position detection done by triangle*/
			// better results
			fPoint pt = {coords.k_pos_x, coords.k_pos_z};
			fPoint v1 = {tri.x1, tri.z1};
			fPoint v2 = {tri.x2, tri.z2};
			fPoint v3 = {tri.x3, tri.z3};

			if( IsPointInTri(pt, v1, v2, v3) )
			{
#endif
				/* location found */
				result = true;
				face = tri;
				//terrain_collision_detect();
				break;
			}
		}
	}

	return result;
}

/* calculate the ray intersection of object (coords) in the triangle (face) */
void WorldMovement::terrain_collision_detect()
{
	float pos_y;

	/* barycentric coordinate calculation ? */
	float A = face.y1 *(face.z2 - face.z3) + face.y2 *(face.z3 - face.z1) + face.y3 *(face.z1 - face.z2);
	float B = face.z1 *(face.x2 - face.x3) + face.z2 *(face.x3 - face.x1) + face.z3 *(face.x1 - face.x2);
	float C = face.x1 *(face.y2 - face.y3) + face.x2 *(face.y3 - face.y1) + face.x3 *(face.y1 - face.y2);
	float D = -(face.x1 *(face.y2* face.z3 - face.y3* face.z2) + face.x2 *(face.y3* face.z1 - face.y1* face.z3) + face.x3 *(face.y1 *face.z2 - face.y2 *face.z1));

	/* plane equation:
	 * ax + by + cz + d = 0
	 * -by = ax + 0 + cz + d
	 * -y = ax + cz + d / b
	 */
	if (B==0) B=0.1; // prevent divide by zero

	pos_y = OBJECT_HEIGHT - ( (A*coords.k_pos_x) + (C*coords.k_pos_z) + D ) / B;
	set_height(pos_y);

	return;
}

void WorldMovement::bouning_box_coordinate_update()
{
	IObjectFactory * pIObj_Factory = (IObjectFactory*)pObjectFactory;

	BoundingBoxStruct *BBox;

	IObjectFactoryClient* pObj_Factory_Entry = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryEntryPtr(LANDSCAPE_OBJECT_INDEX);

	BBox = (BoundingBoxStruct*)pObj_Factory_Entry->fetch_boundingbox_data();


}

void WorldMovement::apply_gravity()
{

}

/* check that the direction is not too steep to climb
 * maximum angle of climb is compile time constant
 *
 * should include walls and boxes*/
void WorldMovement::angle_of_climb()
{





}



/*SET COORDINATES*/

void WorldMovement::set_height(float pos_y)
{
	coords.k_pos_y = pos_y;
}

void WorldMovement::move_forward()
{
	coords.k_pos_x -= (float)sin(coords.m_rot_y*piover180) * move_rate;
	coords.k_pos_z -= (float)cos(coords.m_rot_y*piover180) * move_rate;
//	coords.k_pos_y -= (float)tan(coords.m_rot_x*piover180) * move_rate;
}

void WorldMovement::move_backwards()
{
	coords.k_pos_x += (float)sin(coords.m_rot_y*piover180) * move_rate;
	coords.k_pos_z += (float)cos(coords.m_rot_y*piover180) * move_rate;
//	coords.k_pos_y += (float)tan(coords.m_rot_x*piover180) * move_rate;
}

void WorldMovement::turn_left()
{
	if( coords.m_rot_y >= 359 )
		coords.m_rot_y = 0;
	else
		coords.m_rot_y += rot_rate;
		//coords.m_rot_y -= rot_rate;
}

void WorldMovement::turn_right()
{
	if( coords.m_rot_y <= 0 )
		coords.m_rot_y = 359;
	else
		coords.m_rot_y -= rot_rate;
		//coords.m_rot_y += rot_rate;
}

void WorldMovement::look_up()
{
	if( coords.m_rot_x <= LIMIT_LOOK_UP )
		coords.m_rot_x = LIMIT_LOOK_UP;
	else
		coords.m_rot_x -= rot_rate;
}

void WorldMovement::look_down()
{
	if( coords.m_rot_x >= LIMIT_LOOK_DOWN )
		coords.m_rot_x = LIMIT_LOOK_DOWN;
	else
		coords.m_rot_x += rot_rate;
}


/*LOCALS*/
float Sign(fPoint p1, fPoint p2, fPoint p3)
{
	return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.z - p3.z);
}


bool IsPointInTri(fPoint pt, fPoint v1, fPoint v2, fPoint v3)
{
	bool b1, b2, b3;
	b1 = Sign(pt, v1, v2) < 0.0f;
	b2 = Sign(pt, v2, v3) < 0.0f;
	b3 = Sign(pt, v3, v1) < 0.0f;
	return ((b1 == b2) && (b2 == b3));
}


