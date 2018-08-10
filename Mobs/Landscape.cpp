/*
 * Landscape.cpp
 *
 *  Created on: Feb 17, 2013
 *      Author: robert
 */

#include "../Models/loadmodel.h"
#include "../Tools/debug_output.h"


#include "Landscape.h"

Landscape::Landscape(MobParamaters mobParms) : MobArchTypeClass(mobParms)
{

}

Landscape::~Landscape()
{

}



/* Not sure why I would need a seperate process thread
 * for the landscape but this way cuts out a lot of redundant code
 * because of base class reuse*/
void Landscape::ProcessFunction(){



}

