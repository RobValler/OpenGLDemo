/*
 * Events.cpp
 *
 *  Created on: May 27, 2013
 *      Author: robert
 */

#include <string>
#include <pthread.h>
#include <x86_64-linux-gnu/bits/pthreadtypes.h>   // for declaration of pthread_t, pthread_attr_t

#include "../ObjectFactory/IObjectFactoryClient.h"
#include "../ObjectFactory/IObjectFactory.h"


#include <glm/glm.hpp>
#include <vector>
#include "../Models/ModelDataStruct.h"

#include "../Tools/debug_output.h"
#include "Events.h"


#include <thread>
#include <chrono>


static void *ProcessLoop(void *);

static pthread_t events_thread;

static IObjectFactory * pIObj_Factory;

Events::Events(void * pObj)
{

	pthread_attr_t attr;

	pIObj_Factory = (IObjectFactory*)pObj;


	areWeBeingDestroyed = false;

	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_create( &events_thread, &attr, ProcessLoop, (void *)this );

}

Events::~Events()
{
	areWeBeingDestroyed = true;
	pthread_join( events_thread, NULL );
	report("Events thread destroyed\n");
}


void Events::bounding_box_collision()
{

#define FIRST_OBJ	1

	unsigned int ObjF_index_Primary, ObjF_index_Secondary;
	IObjectFactoryClient *pObj_Factory_Entry_Primary, *pObj_Factory_Entry_Secondary;
	BoundingBoxStruct *BBox_Primary, *BBox_Secondary;

	for( ObjF_index_Primary = FIRST_OBJ; ObjF_index_Primary < pIObj_Factory->fetch_ObjectFactorySize(); ObjF_index_Primary++ )
	{

		// fetch the object used for the primary comparison
		pObj_Factory_Entry_Primary = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryEntryPtr(ObjF_index_Primary);
		BBox_Primary = (BoundingBoxStruct*)pObj_Factory_Entry_Primary->fetch_boundingbox_data();

		for( ObjF_index_Secondary = FIRST_OBJ; ObjF_index_Secondary < pIObj_Factory->fetch_ObjectFactorySize(); ObjF_index_Secondary++ )
		{
			// don't compare an object to itself
			if(ObjF_index_Primary != ObjF_index_Secondary)
			{
				// fetch the data used as the secondary comparison
				pObj_Factory_Entry_Secondary = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryEntryPtr(ObjF_index_Secondary);
				BBox_Secondary = (BoundingBoxStruct*)pObj_Factory_Entry_Secondary->fetch_boundingbox_data();


				// check for intersection
				if ( ( BBox_Primary->min_x < BBox_Secondary->max_x ) && ( BBox_Primary->max_x > BBox_Secondary->min_x ) &&
					 ( BBox_Primary->min_y < BBox_Secondary->max_y ) && ( BBox_Primary->max_y > BBox_Secondary->min_y ) &&
					 ( BBox_Primary->min_z < BBox_Secondary->max_z ) && ( BBox_Primary->max_z > BBox_Secondary->min_z ) )
				{

					// report/notify collision detected
					report("Collision detected between %d and %d\n", ObjF_index_Primary, ObjF_index_Secondary);


				}


			}
		}
	}
}



void Events::ProcessFunction()
{
	while (areWeBeingDestroyed == false)
	{
		//report("Events thread output\n");
        std::this_thread::sleep_for(std::chrono::microseconds(200000));


		// list process services
		bounding_box_collision();


	} // end while

	// tidy up stuff before thread exit
	//...

}

void *ProcessLoop(void * ptr)
{
	Events* pRef = static_cast<Events*>(ptr);
	pRef->ProcessFunction();
	return ptr;
}
