/*
 * MobCitizen.cpp
 *
 *  Created on: Dec 23, 2012
 *      Author: robert
 */


#include "../Tools/debug_output.h"
#include "MobCitizen.h"

#include <thread>
#include <chrono>


Mob_Citizen::Mob_Citizen(MobParamaters mobParms) : MobArchTypeClass(mobParms)
{
	pObjectFactory = mobStats.pObjF;


}


void Mob_Citizen::ProcessFunction() {

//	int i;

	bool location_found = false;

	// point to the correct object within the object factory


	/*TEST THREAD*/
//	unsigned int x=0;

	while (areWeBeingDestroyed == false)
	{


		//report("%s thread output = %d\n", mobStats.objectName, x++);


		turn_right();
		move_forward();
		move_forward();
		//move_forward();

		if ( location_ref_triangle() )
		{
			terrain_collision_detect();
		}

        std::this_thread::sleep_for(std::chrono::microseconds(20000));


	} // end while



}
