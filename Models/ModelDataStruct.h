/*
 * 3DModelDataStruct.h
 *
 *  Created on: Feb 27, 2013
 *      Author: robert
 *
 *      this structure is a container for:
 *      - vertices
 *      - uvs
 *      - normals
 *      - texture mapping image data
 *      - image mapping x coords
 *      - image mapping y coords
 *
 */

#ifndef MODELDATASTRUCT_H_
#define MODELDATASTRUCT_H_

#include <string>

	typedef struct _BoundingBoxStruct
	{
		float min_x;
		float min_y;
		float min_z;

		float max_x;
		float max_y;
		float max_z;

	} BoundingBoxStruct;

	typedef struct _ImageDataStruct
	{
		std::string imageFileName;
	    unsigned long sizeX;	// Images height
	    unsigned long sizeY;	// Images width
	    char *data;				// Image data: size allocated after image size has been determined

	}ImageDataStruct;

	typedef struct _MeshDataStruct
	{
		bool collision_status;
		std::vector <glm::vec3> vertices;
		std::vector <glm::vec2> uvs;
		std::vector <glm::vec3> normals;

	} MeshDataStruct;

	typedef struct _ModelDataStruct
	{
		std::string modelFileName;
		std::vector <ImageDataStruct> images;
		std::vector <MeshDataStruct> meshes;
		BoundingBoxStruct BBox;

		int mesh_count;
		float height;

	} ModelDataStruct;


#endif /* MODELDATASTRUCT_H_ */
