/*
 * MobCitizen.h
 *
 *  Created on: Dec 23, 2012
 *      Author: robert
 */

#ifndef MOBCITIZEN_H_
#define MOBCITIZEN_H_


#include "MobArchTypeClass.h"

class Mob_Citizen: public MobArchTypeClass {
public:
	Mob_Citizen(MobParamaters);
	~Mob_Citizen(){};


protected:
	void ProcessFunction();

};

#endif /* MOBCITIZEN_H_ */
