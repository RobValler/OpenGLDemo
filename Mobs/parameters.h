/*
 * parameters.h
 *
 *  Created on: Jan 7, 2013
 *      Author: robert
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_


typedef struct _MobParamaters
{
	/* location */
	float x_coord;
	float y_coord;
	float z_coord;

	/* statistics */
	int health;
	int armour;

	/* behaviour */
	int threatLevel;
	int visibilityLevel;
	int perceptionLevel;

	/*data*/
	int ObjTypeID;
	char objectName[100];
	char modelFileName[100];

	/*misc*/
	void* pObjF;	// pointer to the object factory


}MobParamaters;


#endif /* PARAMETERS_H_ */
