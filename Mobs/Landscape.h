/*
 * Landscape.h
 *
 *  Created on: Feb 17, 2013
 *      Author: robert
 */

#ifndef LANDSCAPE_H_
#define LANDSCAPE_H_

#include "MobArchTypeClass.h"

class Landscape : public MobArchTypeClass {
public:
	Landscape(MobParamaters mobParms);
	~Landscape();

protected:

	void ProcessFunction();

};

#endif /* LANDSCAPE_H_ */
