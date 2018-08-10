/*
 * ObjectFactory.h
 *
 *  Created on: Jan 5, 2013
 *      Author: robert
 */

#ifndef OBJECTFACTORY_H_
#define OBJECTFACTORY_H_


#include <vector>
#include "../Models/loadmodel.h"
#include "../Mobs/parameters.h"
#include "../Mobs/MobArchTypeClass.h"

#include "IObjectFactoryClient.h"
#include "IObjectFactory.h"

class ObjectFactory : public IObjectFactory{
public:
	ObjectFactory();
	~ObjectFactory();

	// public methods
	void *fetch_ObjectFactoryEntryPtr(int index);
	void* fetch_PlayerObjectPtr();
	void* fetch_ObjectFactoryPlayerEntryPtr();
	unsigned int fetch_ObjectFactorySize();

private:
	int populate_list();
	void instantiate_list();
	void add_object(void * pObj);
	void delete_object(int index);
	void delete_object(void * pObj);
	void delete_all();



protected:

    std::vector <IObjectFactoryClient*> ObjList;
    int player_entry_index;
    void* pObj_Player;


};

#endif /* OBJECTFACTORY_H_ */
