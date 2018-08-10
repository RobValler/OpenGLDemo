/*
 * Events.h
 *
 *  Created on: May 27, 2013
 *      Author: robert
 */

#ifndef EVENTS_H_
#define EVENTS_H_

class Events {
public:
	Events(void * pObj);
	virtual ~Events();


	void bounding_box_collision();
	void ProcessFunction();

protected:
	bool areWeBeingDestroyed;

};

#endif /* EVENTS_H_ */
