
#include <fstream>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <glm/glm.hpp>
#include <algorithm>

#include <iostream>


#include "../Tools/debug_output.h"
#include "../Mobs/parameters.h"

#include "loadmodel.h"
#include "ModelDataStruct.h"

#define BMP_HEADER_SIZE			54

// paths
#define DEFAULT_ROOT_PATH		"../OpenGLDemo/Data/blender/"
#define DEFAULT_MODEL_PATH(x)	DEFAULT_ROOT_PATH + x + "/"
#define DEFAULT_IMAGE_PATH(x)	DEFAULT_ROOT_PATH + x + "/textures/"
#define MTL_EXTENSION			".mtl";
#define OBJ_EXTENSION			".obj";
#define IMG_EXTENSION			".bmp"

// local prototypes
static int loadMTL(const char * filename);
static int loadImage(void* pObj );
void find_minmax_coord(float, float *, float *);


typedef std::vector <std::string> VectOfStr;
static VectOfStr newmtl_list;
static VectOfStr map_Kd_list;


bool loadOBJ(void * pObj)
{

	unsigned int i;
	bool texture_coordinates_present_flag = false;
	bool EOF_REACHED;

	FILE *file = NULL;

	// STD
	std::string LastLineHeader = "X";
	std::string FullFileName;
	std::vector < unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector < glm::vec3 > temp_vertices;
	std::vector < glm::vec2 > temp_uvs;
	std::vector < glm::vec3 > temp_normals;

	// cast pointer of parameter to something useful
	ModelDataStruct *ModelData = (ModelDataStruct*)pObj;
	ImageDataStruct ImgData;

	// pre loop init

	// bounding box init
	ModelData->BBox.min_x = 10000000000.0f;
	ModelData->BBox.min_y = 10000000000.0f;
	ModelData->BBox.min_z = 10000000000.0f;
	ModelData->BBox.max_x = -10000000000.0f;
	ModelData->BBox.max_y = -10000000000.0f;
	ModelData->BBox.max_z = -10000000000.0f;

	ModelData->mesh_count = 0;
	ImgData.data = NULL;

	EOF_REACHED = false;
	FullFileName = DEFAULT_MODEL_PATH(ModelData->modelFileName) + ModelData->modelFileName + OBJ_EXTENSION;


	file = fopen(FullFileName.c_str(), "rt");
	if (file == NULL)
	{
		report("Cannot open %s\n", FullFileName.c_str());
		return false;
	}

	// I want to ........ LOOP IT !!!!
	while( true )
	{
	    char lineHeader[128];
	    // read the first word of the line
	    int result = fscanf(file, "%s", lineHeader);
	    if (result == EOF)
	    {
	    	EOF_REACHED = true;
	        //break;
	    }


		/* INDEX THE DATA */
		if ( (EOF_REACHED == true) ||
			( strcmp( lineHeader, "usemtl" ) == 0 ) )
		{

			if (LastLineHeader == "f" )
			{
				LastLineHeader = "X";

				/*	Index the file so that it can be read by OpenGL
				*	for each vertex of each triangle.
				*/

				MeshDataStruct MeshData;

				// vertices
				if(vertexIndices.size() % 3 == 0)
				{
					for( i=0; i < vertexIndices.size(); i++)
					{
						unsigned int vertexIndex = vertexIndices[i];
						glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
						MeshData.vertices.push_back(vertex);

						// build the bounding box values
						find_minmax_coord(vertex.x, &ModelData->BBox.min_x, &ModelData->BBox.max_x);
						find_minmax_coord(vertex.y, &ModelData->BBox.min_y, &ModelData->BBox.max_y);
						find_minmax_coord(vertex.z, &ModelData->BBox.min_z, &ModelData->BBox.max_z);
					}
				}
				else
				{
					report("Invalid number of vertex indices\n");
					return false;
				}

				// uvs
				if(texture_coordinates_present_flag == true)
				{
					if(uvIndices.size() % 3 == 0)
					{
						for( i=0; i < uvIndices.size(); i++)
						{
							unsigned int uvIndex = uvIndices[i];
							glm::vec2 uvs = temp_uvs[ uvIndex-1 ];
							MeshData.uvs.push_back(uvs);
						}
					}
					else
					{
						report("Invalid number of UV indices\n");
						return false;
					}
				}

				// normals
				if(normalIndices.size() % 3 == 0)
				{
					for( i=0; i < normalIndices.size(); i++)
					{

						unsigned int normalndex = normalIndices[i];
						glm::vec3 normals = temp_normals[ normalndex-1 ];
						MeshData.normals.push_back(normals);
					}
				}
				else
				{
					report("Invalid number of normal indices\n");
					return false;
				}

				// write the data to the ModelData Vector
				ModelData->meshes.push_back(MeshData);
				ModelData->mesh_count++;

			}

			if(EOF_REACHED == true)
			{
				// write the other data
				ModelData->height = ModelData->BBox.max_y - ModelData->BBox.min_y;


				// clear up before exit
				newmtl_list.clear();
				map_Kd_list.clear();

				break;
			}

		}


	    /*MTL LIB*/
	    if ( strcmp( lineHeader, "mtllib" ) == 0 )
	    {
	    	char mtllib_name[100];
	    	fscanf(file, "%s\n", mtllib_name);
	    	report("Loading Blender MTL file = %s\n", mtllib_name);
	    	FullFileName = DEFAULT_MODEL_PATH(ModelData->modelFileName) + mtllib_name;
	    	loadMTL(FullFileName.c_str());

	    }
	    /* NAME */
	    else if ( strcmp( lineHeader, "o" ) == 0 )
	    {
	    	// report the name of the object;
	    	char object_name[100];
	    	fscanf(file, "%s\n", object_name);
	    	report("Object Name = %s\n", object_name);
	    }
		/* VERTEX */
		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}
		/* TEXTURE COORDINATES */
		else if ( strcmp( lineHeader, "vt" ) == 0 )
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			temp_uvs.push_back(uv);
			texture_coordinates_present_flag = true;
		}
		/* NORMALS */
		else if ( strcmp( lineHeader, "vn" ) == 0 )
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}
		/* MATERIAL NAME */
		else if ( strcmp( lineHeader, "usemtl" ) == 0 )
		{
			/*
			* using the "usemtl" as a reference, find the name of the image file required.
			* if "usemtl" is defined in the .obj file (this 'else if' statement here)
			* then the image's filename can be found in the corresponding .mtl file.
			* the .mtl file should be already loaded from the 'mtllib' section above
			*/

			int index;
			size_t lastslash, dot;
			char usemtl_name[100];
			std::string tmp, tmp2, tmp3;

			fscanf(file, "%s\n", usemtl_name);
			//report("usemtl = %s\n", usemtl_name);

			// see if the 'usemtl' reference can be found in the previously loaded MTL file data
			for( index = 0 ; index < (int)newmtl_list.size() ; index++ )
			{

				tmp = newmtl_list[index];
				if( -1 != tmp.find(usemtl_name, 0))
				{
					tmp2 = map_Kd_list[index];

					// find the name between the last '/' and the '.' (or '.png')
					lastslash = tmp2.find_last_of('/')+1;		// get the read pos
					dot = tmp2.find_last_of('.') - lastslash;	// just need the length here
					tmp3 = tmp2.substr(lastslash, dot);

					ImgData.imageFileName = DEFAULT_IMAGE_PATH(ModelData->modelFileName) + tmp3 + IMG_EXTENSION;
					std::cout << "usemtl = " << usemtl_name << ", Image filename = " << ImgData.imageFileName << "\n";

					// load the image
					if (!loadImage(&ImgData))
						exit(1);

					ModelData->images.push_back(ImgData);
					break;

				} // end if
			} // end for



		}
		/* GROUP NAME */
		else if ( strcmp( lineHeader, "g" ) == 0 )
		{
			/*TEMP CODE*/
			ImgData.imageFileName = DEFAULT_IMAGE_PATH(ModelData->modelFileName) + "texture2" + IMG_EXTENSION;
			if (!loadImage(&ImgData))
				exit(1);
			ModelData->images.push_back(ImgData);

		}
		/* SMOOTH SHADING */
		else if ( strcmp( lineHeader, "s" ) == 0 )
		{


		}
		/* FACE DEFINITIONS */
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			if(texture_coordinates_present_flag == true)
			{
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
											&vertexIndex[0], &uvIndex[0], &normalIndex[0],
											&vertexIndex[1], &uvIndex[1], &normalIndex[1],
											&vertexIndex[2], &uvIndex[2], &normalIndex[2] );

				if (matches != 9)
				{
					if (matches == -1)
					{
						report("WaveFront.OBJ File read error in section (f) !\n");
					}
					else
					{
						report("Problem reading the file, only %d matches out of 9 found\n", matches);
					}
				}
				else
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}
			}
			else
			{
				int matches = fscanf(file, "%d//%d %d//%d %d//%d\n",
											&vertexIndex[0], &normalIndex[0],
											&vertexIndex[1], &normalIndex[1],
											&vertexIndex[2], &normalIndex[2] );

				if (matches != 6)
				{
					report("File read error, only %d matches out of 6 found\n", matches);
					//return false;
				}
				else
				{
					vertexIndices.push_back(vertexIndex[0]);
					vertexIndices.push_back(vertexIndex[1]);
					vertexIndices.push_back(vertexIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);
				}

			}

			LastLineHeader = "f";

		} // end if header = "f"



	} // finished reading the file

	fclose(file);

	return true;
}


typedef struct {
    char filetype[2];
    int filesize;
    short reserved1;
    short reserved2;
    int dataoffset;

} file_header;

typedef struct {
    file_header  fileheader;
    unsigned int headersize;
    int          width;
    int          height;
    short        planes;
    short        bitsperpixel;
    unsigned int compression;
    unsigned int bitmapsize;
    int          horizontalres;
    int          verticalres;
    unsigned int numcolors;
    unsigned int importantcolors;

} bitmap_header;


/*
 * Load the image for the 3D model
 * This is an uncompressed, windows bitmap (.BMP) format
 *
 * */
int loadImage(void* pObj )
{
    FILE *file;
    unsigned long i;
    char temp;
	bitmap_header header;
	unsigned char header_data[BMP_HEADER_SIZE];

	// cast injected data
	ImageDataStruct *ImageData = (ImageDataStruct*)pObj;

	if ((file = fopen(ImageData->imageFileName.c_str(), "rb"))==NULL)
	{
		report("Image file Not Found\n");
		return 0;
	}

	if (fread(header_data, sizeof(unsigned char), BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE)
	{
		report("Error reading image header file information for\n");
		return 0;
	}

	// read the contents of the image file header
	header.fileheader.dataoffset = *(int*)&header_data[10];
	header.width = *(int*)&header_data[18];
	header.height = *(int*)&header_data[22];
	header.planes = *(short*)&header_data[26];
	header.bitsperpixel = *(short*)&header_data[28];
	//header.compression
	header.bitmapsize = *(int*)&header_data[34];

#if 0
	// report the contents of the image file header
	report("Data offset : %d\n", header.fileheader.dataoffset);
	report("Image width : %d\n", header.width);
	report("Image height : %d\n", header.height);
	report("Image planes : %d\n", header.planes);
	report("Image bitsperpixel : %d\n", header.bitsperpixel);
	report("Image bitmapsize : %d\n", header.bitmapsize);
#endif

	if( header.bitsperpixel != 24 )
	{
		report("loadImage: Warning: Image NOT 24bpp, bits per pixel = %d\n", header.bitsperpixel);
	}

	// read the data.
	ImageData->sizeX = header.width;
	ImageData->sizeY = header.height;
	//ImageData->data = (char *) malloc(header.bitmapsize);
	ImageData->data = new char [header.bitmapsize];

	if (ImageData->data == NULL)
	{
		report("Failed to allocate memory for image data");
		return 0;
	}

	fseek(file, header.fileheader.dataoffset, SEEK_SET);

	if ((i = fread(ImageData->data, header.bitmapsize, 1, file)) != 1)
	{
		report("Error reading image data\n");
		return 0;
	}

	// reverse all of the colors. (bgr -> rgb)
	for (i=0; i<header.bitmapsize; i+=(header.bitsperpixel / 8))
	{
		temp = ImageData->data[i];
		ImageData->data[i] = ImageData->data[i+2];
		ImageData->data[i+2] = temp;
	}

	fclose(file);

    return 1;
}



int loadMTL(const char * filename)
{

    FILE *file;
    char lineHeader[256];

	if ((file = fopen(filename, "rt"))==NULL)
	{
		report("MTL file Not Found\n");
		return -1;
	}

    while(true)
    {

	    if( NULL == fgets( lineHeader, sizeof(lineHeader), file) )
		{
			break;
		}

		if(( lineHeader[0] == '#' ) ||
		   ( lineHeader[0] == '\n' ) )
		{
			// ignore comment
		}
		else
		{
			std::string readline = lineHeader;
			size_t space = readline.find(" ", 0);

			if( readline.substr(0, space ) == "newmtl" )
			{
				newmtl_list.push_back( readline.substr( space + 1, 100 ) );
			}
			if( readline.substr(0, space ) == "map_Kd" )
			{
				map_Kd_list.push_back( readline.substr( space + 1, 100 ) );
			}
		}
    }

	fclose(file);

    // ToDo - check that both vectors are the same length


	return 0;
}

void find_minmax_coord(float ref, float *min, float *max)
{
	if(ref < *min)
	{
		*min = ref;
	}
	else if(ref > *max)
	{
		*max = ref;
	}
}


