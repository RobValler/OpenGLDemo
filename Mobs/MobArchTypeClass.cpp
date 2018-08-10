/*
 * MobArchTypeClass.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: robert
 */

#include <string.h>

#include "MobArchTypeClass.h"
#include "../Models/loadmodel.h"

#include "../Tools/debug_output.h"



static void *ProcessLoop(void * ptr);

MobArchTypeClass::MobArchTypeClass(MobParamaters mobParms) : mobStats(mobParms)
{

	/*INITIALISE*/
	pthread_attr_t attr;

	coords.k_pos_x = mobStats.x_coord;
	coords.k_pos_y = mobStats.y_coord;
	coords.k_pos_z = mobStats.z_coord;
	coords.m_rot_x = 0;
	coords.m_rot_y = 0;
	coords.m_rot_z = 0;

	areWeBeingDestroyed = false; // NOT THREAD SAFE....maybe

	report("========== Init [%s] ==========\n", mobStats.objectName);
	ModelData.modelFileName = mobStats.modelFileName;
	loadOBJ(&ModelData);
    report("========== Init Finished ==========\n");

	// set general thread attributes and start it
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_create( &processing_thread, &attr, ProcessLoop, (void *)this );



	/*"ProcessLoop" is the calling a pure virtual, the concrete of which is defined
	 * by derived classes ... anything called after this line caused the following problem:
	 *
	 * pure virtual method called
	 * terminate called without an active exception
	 *
	 * ToDo :  INVESTIGATE !!
	 *
	 * */

	//sleep(1);

}

MobArchTypeClass::~MobArchTypeClass()
{
//	unsigned int i;

	// we need to notify any threads that we are being destroyed
	areWeBeingDestroyed = true;

	/* free used memory*/
	// image data - handled by the opengl renderer for now
#if 0
	for( i = 0; i < ModelData.images.size() ; i++ )
	{
		delete ModelData.images[i].data;
	}
#endif

	//model list
	ModelData.images.clear();

	// wait for the processing thread to finish
	pthread_join( processing_thread, NULL );

	report("Object [%s] was destroyed!\n", mobStats.objectName);
}

void *MobArchTypeClass::fetch_vertices(int i)
{
	return &ModelData.meshes[i].vertices;
}

void *MobArchTypeClass::fetch_uvs(int i)
{
	return &ModelData.meshes[i].uvs;
}

void *MobArchTypeClass::fetch_normals(int i)
{
	return &ModelData.meshes[i].normals;
}

void *MobArchTypeClass::fetch_ImagePtr(int i)
{
	return &ModelData.images[i];
}

void *MobArchTypeClass::fetch_coordinates()
{
	return &coords;
}

int MobArchTypeClass::fetch_model_count()
{
	return ModelData.mesh_count;
}

int MobArchTypeClass::fetch_image_count()
{
	return ModelData.images.size();
}

bool MobArchTypeClass::fetch_collision_status()
{
	return true;
}

int MobArchTypeClass::fetch_object_type()
{
	return mobStats.ObjTypeID;
}

void *MobArchTypeClass::fetch_boundingbox_data()
{
	return &ModelData.BBox;
}

void MobArchTypeClass::delete_image(int i)
{
	delete ModelData.images[i].data;
	//ModelData.images.erase(ModelData.images.begin()+i);
}


/* PROCESS */
static void *ProcessLoop(void * ptr)
{
	MobArchTypeClass* pRef = static_cast<MobArchTypeClass*>(ptr);
	pRef->ProcessFunction();
	return ptr;
}



