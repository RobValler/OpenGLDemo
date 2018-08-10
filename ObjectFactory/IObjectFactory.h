/*
 * IObjectFactory.h
 *
 *  Created on: Feb 17, 2013
 *      Author: robert
 */

#ifndef IOBJECTFACTORY_H_
#define IOBJECTFACTORY_H_

class IObjectFactory {
public:
	IObjectFactory(){};
	virtual ~IObjectFactory(){};

	virtual void* fetch_ObjectFactoryEntryPtr(int index)=0;
	virtual void* fetch_PlayerObjectPtr()=0;
	virtual void* fetch_ObjectFactoryPlayerEntryPtr()=0;
	virtual unsigned int fetch_ObjectFactorySize()=0;
};

#endif /* IOBJECTFACTORY_H_ */
