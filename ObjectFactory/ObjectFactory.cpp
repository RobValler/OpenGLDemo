/*
 * ObjectFactory.cpp
 *
 *  Created on: Jan 5, 2013
 *      Author: robert
 */

// system includes
#include <fstream>
#include <cstring>
#include <stdlib.h>

// reference all the mob class header files
#include "../Mobs/Landscape.h"
#include "../Mobs/Player.h"
#include "../Mobs/MobCitizen.h"
#include "IObjectFactoryClient.h"

#include "../Tools/debug_output.h"


#include "ObjectFactory.h"

#define MODEL_TYPE_LANDSCAPE	0
#define MOBTYPE_PLAYER			1
#define MOBTYPE_C_NORM			2
#define MOBTYPE_C_HUNT			3
#define MOBTYPE_C_NUT			4
#define MOBTYPE_P_CONS			5
#define MOBTYPE_P_SARG			6
#define MOBTYPE_P_INSP			7


ObjectFactory::ObjectFactory()
{
	report("Instantiate the object factory\n");

	player_entry_index = -1;


	populate_list();
}

ObjectFactory::~ObjectFactory()
{
	report("Shutting down the object factory\n");
	this->delete_all();
}


/*
 *
 * */
int ObjectFactory::populate_list()
{
	MobParamaters mobParms;

	// load the level data
    char filename[]="/home/rob/WORK/Development/OpenGLDemo/Data/level_1.lvl";
    FILE *file;

    if ((file = fopen(filename, "rt"))==nullptr)
	{
		report("File Not Found : %s\n",filename);
        exit (EXIT_FAILURE);
	}

	while( true )
	{

	    char lineHeader[256];
	    if( NULL == fgets( lineHeader, sizeof(lineHeader), file) )
		{
			break;
		}

		if(( lineHeader[0] == '#' ) ||
		   ( lineHeader[0] == '\n' ) )
		{
			//report("-----------------------comment ignored\n");
		}
		else
		{
			// read the parameters from the file

			memset(&mobParms, 0, sizeof(mobParms)); //clean each time, not all parameters are always set

	        sscanf(lineHeader, "%d %s %s %f %f %f %d %d %d %d %d %s \n",	&mobParms.ObjTypeID,
																			&mobParms.objectName,
																			&mobParms.modelFileName,
																			&mobParms.x_coord,
																			&mobParms.y_coord,
																			&mobParms.z_coord,
																			&mobParms.health,
																			&mobParms.armour,
																			&mobParms.threatLevel,
																			&mobParms.perceptionLevel,
																			&mobParms.visibilityLevel);


	        mobParms.pObjF = this;

	        // now instantiate the object
	        switch(mobParms.ObjTypeID)
	        {
	        case MODEL_TYPE_LANDSCAPE:
	        {
	        	add_object( new Landscape(mobParms) );
	        	break;
	        }
	        case MOBTYPE_PLAYER:
	        {
	        	pObj_Player = new Player(mobParms);
	        	add_object( pObj_Player );
	        	player_entry_index = fetch_ObjectFactorySize() - 1;
		        break;
	        }
	        case MOBTYPE_C_NORM:
	        {
	        	add_object( new Mob_Citizen(mobParms) );
		        break;
	        }
	        case MOBTYPE_C_HUNT:
	        case MOBTYPE_C_NUT:
	        case MOBTYPE_P_CONS:
	        case MOBTYPE_P_SARG:
	        case MOBTYPE_P_INSP:
	        default:
	        	break;
	        }

	    } // end if

	} // end while

	report("ObjectFactory::populate_list() - Number of objects are: %d\n", ObjList.size());

	fclose(file);

    return 1;
}

void ObjectFactory::instantiate_list()
{

}

void ObjectFactory::add_object(void* pObj)
{
	ObjList.push_back( (IObjectFactoryClient*)pObj );
}

void ObjectFactory::delete_object(int index)
{
	delete ObjList[index];
	ObjList.erase(ObjList.begin()+index);
}

void ObjectFactory::delete_object(void * pObj)
{
	int i;

	for (i=0; i< ObjList.size(); i++)
	{
		if(ObjList[i] == (IObjectFactoryClient*)pObj)
		{
			delete ObjList[i];
			ObjList.erase(ObjList.begin()+i);
		}
	}
}

void ObjectFactory::delete_all()
{
	int i;

	// cycle through the list and free memory of instantiated objects
	// landscape is loaded first on startup, kill it last

	for (i=1/*skip Landscape*/; i < ObjList.size(); i++)
	{
		delete ObjList[i];
	}

	/* we must kill the landscape last
	 * since it is still being read for
	 * terrain collision until complete shutdown*/
	delete ObjList[0];


	// clear the list of all pointer references
	ObjList.clear();
}

// public methods
void* ObjectFactory::fetch_ObjectFactoryEntryPtr(int index)
{
	return (void*)ObjList[index];
}

void* ObjectFactory::fetch_PlayerObjectPtr()
{
	return pObj_Player;
}

void* ObjectFactory::fetch_ObjectFactoryPlayerEntryPtr()
{
	return (void*)ObjList[player_entry_index];
}

unsigned int ObjectFactory::fetch_ObjectFactorySize()
{
	return ObjList.size();
}
