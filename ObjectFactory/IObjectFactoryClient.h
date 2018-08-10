/*
 * IObjectFactoryClient.h
 *
 *  Created on: Feb 17, 2013
 *      Author: robert
 */

#ifndef IOBJECTFACTORYCLIENT_H_
#define IOBJECTFACTORYCLIENT_H_

class IObjectFactoryClient {
public:
	IObjectFactoryClient(){};
	virtual ~IObjectFactoryClient(){};

	virtual void *fetch_vertices(int)=0;
	virtual void *fetch_uvs(int)=0;
	virtual void *fetch_normals(int)=0;
	virtual void *fetch_ImagePtr(int)=0;
	virtual void *fetch_coordinates()=0;
	virtual int fetch_model_count()=0;
	virtual int fetch_image_count()=0;
	virtual bool fetch_collision_status()=0;
	virtual int fetch_object_type()=0;
	virtual void *fetch_boundingbox_data()=0;
	virtual void delete_image(int)=0;

	//virtual bool check_object_location(void*)=0;



};

#endif /* IOBJECTFACTORYCLIENT_H_ */
