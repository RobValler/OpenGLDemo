/*
 ============================================================================
 Name        : OpenGL_DrawScene.cpp
 Author      : 
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */

#define GL_GLEXT_PROTOTYPES

// renderer API
#include <GL/gl.h>				// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library
#include <glm/glm.hpp>			// OpenGL maths

// window manager - GLUT
#include <GL/glut.h>    		// Header File For The GLUT Library
#include <GL/freeglut_ext.h>

#include <vector>

// threads
#include <pthread.h>
#include <x86_64-linux-gnu/bits/pthreadtypes.h>   // for declaration of pthread_t, pthread_attr_t

// object factory
#include "../ObjectFactory/IObjectFactoryClient.h"
#include "../ObjectFactory/IObjectFactory.h"
#include "../Mobs/Player.h"

// models
#include "../Models/ModelDataStruct.h"
#include "../src/WorldMovement.h"

//debug
#include "../Tools/debug_output.h"
#include "common.h"

#include "state_machine.h"

//local header file
#include "OpenGL_DrawScene.h"



#include <thread>
#include <chrono>

/* defines for mouse/keys not defined in GLUT. */
#define ESCAPE 27
#define W_KEY 'w'
#define S_KEY 's'
#define A_KEY 'a'
#define D_KEY 'd'

#define GLUT_MIDDLE_SCROLL_UP		0003
#define GLUT_MIDDLE_SCROLL_DOWN		0004
#define GLUT_M_SIDE_FRONT_BTN		8


// rate of movement
#define rot_rate		1.5f
#define move_rate		0.2f
#define piover180		0.0174532925f

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768
#define SCREEN_NEAR		0.5f
#define SCREEN_FAR		1000.0f


/* GLOBALS */
/* The number of our GLUT window */
static int window;

// input and view variables
int mouse_zoom_value = 0;
int mouse_x_grabpoint = 0;
int mouse_y_grabpoint = 0;

// pointer list of mesh data of the current model
std::vector <glm::vec3> * pVertices;
std::vector <glm::vec2> * pUvs;
std::vector <glm::vec3> * pNormals;

// texture reference list
std::vector <unsigned int> Textures;


/* LOCAL PROTOTYPES */
// OpenGL
static void InitGL(int Width, int Height);
static void ReSizeGLScene(int Width, int Height);
static void DrawGLScene();
static void LoadGLTextures();

// GLUT
static int InitMainDrawingLoop(int argc, char **argv);
static void keyPressed(unsigned char key, int x, int y);
static void mousePressed(int button, int state, int x, int y);
static void mouseDragged(int x, int y);

// OTHER
void API_SetShutdownMode();

/*FLAGS*/
bool forward_toggle = false;

/*GLOBALS*/
static IObjectFactory *pIObj_Factory;
static Player *pPlayer;
static pthread_t drawscene_thread;



DrawScene::DrawScene(void * ptr)
{

	pthread_attr_t attr;

	// parse the parameter data
	args* pArgs = (args*)ptr;
	pIObj_Factory = (IObjectFactory*)pArgs->pIObj_Factory;
	pPlayer = (Player*)pIObj_Factory->fetch_PlayerObjectPtr();

	areWeBeingDestroyed = false;

	// initialise the thread
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_create( &drawscene_thread, &attr, &DrawScene::ProcessFunction, pArgs );

}

void *DrawScene::ProcessFunction(void *ptr)
{
	args* pArgs = (args*)ptr;
	InitMainDrawingLoop(pArgs->argc, pArgs->argv);
	return ptr;
}

DrawScene::~DrawScene()
{
	areWeBeingDestroyed = true;
	pthread_join( drawscene_thread, NULL );
	report("Events thread destroyed\n");
}



/************************
 * 		OpenGL API		*
******************************************************************************************************************************************/
void InitGL(int Width, int Height)
{
	LoadGLTextures();							// Load The Texture(s)
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);						// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
	glShadeModel(GL_SMOOTH);					// Enables Smooth Color Shading
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();							// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height, SCREEN_NEAR, SCREEN_FAR);	// Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW);

}


void ReSizeGLScene(int Width, int Height)
{
	if (Height==0) Height=1;

	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height, SCREEN_NEAR, SCREEN_FAR);
	glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{

	unsigned int texture_index;
	unsigned int i, ObjF_index, ObjF_Entry_Model_Count, ObjF_Entry_Model_Index;
	world_move_coords *Coord, *PlayerCoord;

	IObjectFactoryClient* pObj_Factory_Player_Entry = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryPlayerEntryPtr();
	PlayerCoord = (world_move_coords*)pObj_Factory_Player_Entry->fetch_coordinates();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glLoadIdentity();										// Reset The View

	/*the view and moving the player object*/
	gluLookAt(	0, 10 + (mouse_zoom_value / 2), 10 + mouse_zoom_value,	// eye coordinates
				0, 0, 0,	// look at coordinates
	            0, 1, 0 ); 	// which direction is UP = Y

	GLfloat scene_y = 360.0f - PlayerCoord->m_rot_y;
	GLfloat scene_x = 360.0f - PlayerCoord->m_rot_x;
	glRotatef(-scene_x, 1.0f, 0, 0);		// rotate up and down
	glRotatef(scene_y, 0, 1.0f, 0);		// rotate left and right
	glTranslatef(-PlayerCoord->k_pos_x, -PlayerCoord->k_pos_y, -PlayerCoord->k_pos_z);


	// reset the index for the texture reference list
	texture_index = 0;


	// run through the list of objects in the object factory
	for( ObjF_index = 0; ObjF_index < pIObj_Factory->fetch_ObjectFactorySize(); ObjF_index++ )
	{

		// point to the correct object within the object factory
		IObjectFactoryClient* pObj_Factory_Entry = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryEntryPtr(ObjF_index);

		glPushMatrix();	// push for each object

			glColor3f(1.0f, 1.0f, 1.0f);

			// set coordinates and angles for the object
			Coord = (world_move_coords*)pObj_Factory_Entry->fetch_coordinates();
			if( pObj_Factory_Entry->fetch_object_type() == 0 ) // We are rendering the Landscape
			{
				//glRotatef(Coord->m_rot_y, 0, 1.0f, 0);				// rotate left and right
				//glTranslatef(Coord->k_pos_x, Coord->k_pos_y, Coord->k_pos_z);
			}
			else
			{
				glTranslatef(Coord->k_pos_x, Coord->k_pos_y, Coord->k_pos_z);
				glRotatef(Coord->m_rot_y, 0, 1.0f, 0);		// rotate left and right
			}

			ObjF_Entry_Model_Count = pObj_Factory_Entry->fetch_model_count();

			// run through all the meshes in each object
			for( ObjF_Entry_Model_Index = 0 ; ObjF_Entry_Model_Index < ObjF_Entry_Model_Count ; ObjF_Entry_Model_Index++ )
			{

				// fetch mesh data
				pVertices = (std::vector <glm::vec3> *)pObj_Factory_Entry->fetch_vertices(ObjF_Entry_Model_Index);
				pUvs = (std::vector <glm::vec2> *)pObj_Factory_Entry->fetch_uvs(ObjF_Entry_Model_Index);
				//pNormals = (std::vector <glm::vec3> *)pObj_Factory_Entry->fetch_normals(ObjF_Entry_Model_Index);

				// fetch textures, one per mesh
				glBindTexture( GL_TEXTURE_2D, (unsigned int)Textures[texture_index++] );

				// draw the mesh from individual triangles
				glBegin(GL_TRIANGLES);
					for( i=0; i < pVertices->size(); i++ )
					{
						if(( (i+1) % 3 == 0 ))
						{
							//glNormal3f(pNormals->at(i-2).x, pNormals->at(i-2).y, pNormals->at(i-2).z);
							glTexCoord2f(pUvs->at(i-2).x, pUvs->at(i-2).y);
							glVertex3f( pVertices->at(i-2).x, pVertices->at(i-2).y, pVertices->at(i-2).z );

							//glNormal3f(pNormals->at(i-1).x, pNormals->at(i-1).y, pNormals->at(i-1).z);
							glTexCoord2f(pUvs->at(i-1).x, pUvs->at(i-1).y);
							glVertex3f( pVertices->at(i-1).x, pVertices->at(i-1).y, pVertices->at(i-1).z );

							//glNormal3f(pNormals->at(i).x, pNormals->at(i).y, pNormals->at(i).z);
							glTexCoord2f(pUvs->at(i).x, pUvs->at(i).y);
							glVertex3f( pVertices->at(i).x, pVertices->at(i).y, pVertices->at(i).z );

						} // end if i mod 3

					} // end for mesh
				glEnd();

			}// end for object

		glPopMatrix();

#if 1
		/*DRAW THE BOUNDING BOX*/
		BoundingBoxStruct *BBox = (BoundingBoxStruct*)pObj_Factory_Entry->fetch_boundingbox_data();


		glPushMatrix();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
			glVertex3f(BBox->min_x, BBox->min_y, BBox->min_z);
			glVertex3f(BBox->min_x, BBox->min_y, BBox->max_z);
			glVertex3f(BBox->min_x, BBox->min_y, BBox->min_z);
			glVertex3f(BBox->min_x, BBox->max_y, BBox->min_z);
			glVertex3f(BBox->min_x, BBox->min_y, BBox->min_z);
			glVertex3f(BBox->max_x, BBox->min_y, BBox->min_z);
			glVertex3f(BBox->max_x, BBox->min_y, BBox->min_z);
			glVertex3f(BBox->max_x, BBox->min_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->min_y, BBox->max_z);
			glVertex3f(BBox->min_x, BBox->min_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->min_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->min_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->min_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->min_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->min_x, BBox->max_y, BBox->min_z);
			glVertex3f(BBox->min_x, BBox->max_y, BBox->min_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->min_z);
			glVertex3f(BBox->min_x, BBox->min_y, BBox->max_z);
			glVertex3f(BBox->min_x, BBox->max_y, BBox->max_z);
			glVertex3f(BBox->max_x, BBox->min_y, BBox->min_z);
			glVertex3f(BBox->max_x, BBox->max_y, BBox->min_z);
		glEnd();

		glPopMatrix();
#endif



	}//end for object factory

	// swap buffers to display, since we're double buffered.
	glutSwapBuffers();

}

// Load Bitmaps And Convert To Textures
void LoadGLTextures()
{
	unsigned int texture;
	unsigned int texture_index;
	unsigned int ObjF_index, ObjF_Entry_Image_Count, ObjF_Entry_Model_ImageIndex;

	ImageDataStruct *image = NULL;

	texture_index = 0;

	// rattle through the object factory
	for( ObjF_index = 0; ObjF_index < pIObj_Factory->fetch_ObjectFactorySize(); ObjF_index++ )
	{
		// point to the correct cell in the object factory
		IObjectFactoryClient* pObj_Factory_Entry = (IObjectFactoryClient*)pIObj_Factory->fetch_ObjectFactoryEntryPtr(ObjF_index);
		ObjF_Entry_Image_Count = pObj_Factory_Entry->fetch_image_count();

		for( ObjF_Entry_Model_ImageIndex = 0 ; ObjF_Entry_Model_ImageIndex < ObjF_Entry_Image_Count ; ObjF_Entry_Model_ImageIndex++ )
		{

			image = (ImageDataStruct*)pObj_Factory_Entry->fetch_ImagePtr(ObjF_Entry_Model_ImageIndex);
			if( image == NULL )
			{
				report("LoadGLTextures: invalid image pointer\n");
				API_SetShutdownMode();

			}

			glGenTextures(1, &texture); 				// create the texture
			glBindTexture(GL_TEXTURE_2D, texture);		// bind the 2d texture (x and y size)

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smaller than texture

			// 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
			// border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
			glTexImage2D(GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

			Textures.push_back(texture);	// add to the list
			texture_index++;


			/*we have finished with the loaded images
			 * since they are now bound to the model
			 * we can delete them now to free resources*/
			pObj_Factory_Entry->delete_image(ObjF_Entry_Model_ImageIndex);

		}

	}

	report("LoadGLTextures: number of textures loaded = %d\n", texture_index);

}


/************************
 * 		GLUT API		*
*******************************************************************************************************************************************/

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y)
{

    switch(key)
    {
    	case ESCAPE:
    	{
    	    /* avoid thrashing this procedure */
            std::this_thread::sleep_for(std::chrono::microseconds(100));

    		/* shut down our window */
    		// just exit from the main glut loop and shutdown later
    	    API_SetShutdownMode();
			break;
    	}
    	case W_KEY:
    	{
    		pPlayer->forward();
    		break;
    	}
    	case S_KEY:
    	{
    		pPlayer->backward();
    		break;
    	}
    	case A_KEY:
    	{
    		pPlayer->left();
    		break;
    	}
    	case D_KEY:
    	{
    		pPlayer->right();
    		break;
    	}
    	default:
    		break;
    }

}


void mousePressed(int button, int state, int x, int y)
{

	switch(button)
	{
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_MIDDLE_SCROLL_UP:
			// zoom in
			if( mouse_zoom_value >= 2 )
				mouse_zoom_value -= 2;
			break;
		case GLUT_MIDDLE_SCROLL_DOWN:
			// zoom out
			if( mouse_zoom_value < 30 )
				mouse_zoom_value += 2;
			break;
		case GLUT_M_SIDE_FRONT_BTN:
			// toggle move forward
			if( state == GLUT_UP )
			{
				if( forward_toggle == true)
					forward_toggle = false;
				else
					forward_toggle = true;

				pPlayer->autorun(forward_toggle);
			}
			break;
		case GLUT_MIDDLE_BUTTON:
		case GLUT_RIGHT_BUTTON:
		default:
			return;
	}

	switch(state)
	{
		case GLUT_UP:
			break;
		case GLUT_DOWN:
			mouse_x_grabpoint = x;
			mouse_y_grabpoint = y;
			break;
		default:
			break;
	}

}

void mouseDragged(int x, int y)
{

	if (y > mouse_y_grabpoint )
	{
		pPlayer->down();
	}
	else if (y < mouse_y_grabpoint )
	{
		pPlayer->up();
	}

	if (x > mouse_x_grabpoint )
	{
		pPlayer->right();
	}
	else if (x < mouse_x_grabpoint )
	{
		pPlayer->left();
	}

	mouse_x_grabpoint = x;
	mouse_y_grabpoint = y;

}


int InitMainDrawingLoop(int argc, char **argv)
{

	/* Initialize GLUT state - glut will take any command line arguments that pertain to it or
	X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
	glutInit(&argc, argv);

	/* Select type of Display mode:
	Double buffer
	RGBA color
	Alpha components supported
	Depth buffer */
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

	/* define the window size */
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	/* the window starts at the upper left corner of the screen */
	glutInitWindowPosition(300, 200);

	// set option: when leaving the main glut loop, do nto terminate the process
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	/* Open a window */
	window = glutCreateWindow("TEST GAME ENGINE APP\n");

	/* Register the function to do all our OpenGL drawing. */
	glutDisplayFunc(&DrawGLScene);

	/* Go fullscreen.  This is the soonest we could possibly go fullscreen. */
	//glutFullScreen();

	/* Even if there are no events, redraw our gl scene. */
	glutIdleFunc(&DrawGLScene);

	/* Register the function called when our window is resized. */
	glutReshapeFunc(&ReSizeGLScene);

	/* Register the function called when the keyboard is pressed. */
	//glutKeyboardFunc(&keyPressed);
	glutKeyboardFunc(&keyPressed);

	/*handles mouse button events*/
	glutMouseFunc(&mousePressed);

	/*handles the mouse drag events*/
	glutMotionFunc(&mouseDragged);

	/* Initialize our window. */
	InitGL(SCREEN_WIDTH, SCREEN_HEIGHT);

	/* --== THIS IS THE MAIN LOOP FOR THE GFX RENDERER ==--
	 * glutMainLoop() will block until the application is shutting down.
	 *
	 * */
	glutMainLoop();


	// tidy up before exiting
	Textures.clear();

	return 0;
}




/********************
*  PUBLIC METHODS   *
*******************************************************************************************************************************************/
void* API_RenderingFunc(void * ptr)
{
	args* pArgs = (args*)ptr;
	pIObj_Factory = (IObjectFactory*)pArgs->pIObj_Factory;
	pPlayer = (Player*)pIObj_Factory->fetch_PlayerObjectPtr();
	InitMainDrawingLoop(pArgs->argc, pArgs->argv);
	return ptr;
}

void API_SetShutdownMode()
{
	glutLeaveMainLoop();
	StateMachine::set_state(STATE_SHUTDOWN);

}

