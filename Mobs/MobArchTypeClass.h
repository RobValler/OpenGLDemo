/*
 * MobArchTypeClass.h
 *
 *  Created on: Dec 18, 2012
 *      Author: robert
 */

#ifndef MOBARCHTYPECLASS_H_
#define MOBARCHTYPECLASS_H_

// system includes
#include <pthread.h>
#include <x86_64-linux-gnu/bits/pthreadtypes.h>   // for declaration of pthread_t, pthread_attr_t
#include <vector>
#include <glm/glm.hpp>

// custom includes
#include "../src/WorldMovement.h"
#include "../Mobs/parameters.h"
#include "../Models/ModelDataStruct.h"
#include "../ObjectFactory/IObjectFactoryClient.h"


class MobArchTypeClass : public WorldMovement, IObjectFactoryClient{
public:
	MobArchTypeClass(MobParamaters mobStats);
	~MobArchTypeClass();

	// fetch vector data
	void *fetch_vertices(int);
	void *fetch_uvs(int);
	void *fetch_normals(int);
	void *fetch_ImagePtr(int);
	void *fetch_coordinates();
	int fetch_model_count();
	int fetch_image_count();
	bool fetch_collision_status();
	int fetch_object_type();
	void *fetch_boundingbox_data();
	void delete_image(int);

	virtual void ProcessFunction()=0;

protected:

	pthread_t processing_thread;
	MobParamaters mobStats;
	ModelDataStruct ModelData;
	bool areWeBeingDestroyed;


};

#endif /* MOBARCHTYPECLASS_H_ */
