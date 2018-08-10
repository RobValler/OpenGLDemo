/*
 * state_machine.cpp
 *
 *  Created on: Sep 1, 2013
 *      Author: robert
 */



#include "../Mobs/Player.h"

#include "../ObjectFactory/ObjectFactory.h"
#include "OpenGL_DrawScene.h"
#include "Events.h"

#include "common.h"


#include "../Tools/debug_output.h"

#include "state_machine.h"


#include <thread>
#include <chrono>

// include all C compiled object here
#if defined(__cplusplus)
extern "C"
{
#endif



#if defined(__cplusplus)
}
#endif



/* CLASSES */
static ObjectFactory *pObj_Factory;
static Events *pObj_Events;
static DrawScene *pObj_DrawScene;

static args args_t;
static char current_state;


void StateMachine::setParameters(int argc, char ** argv)
{

	report("Starting Application!\n\n");

	args_t.argc = argc;
	args_t.argv = argv;

	current_state = STATE_UNKNOWN;

}

StateMachine& StateMachine::getInstance()
{
    static StateMachine instance;
    return instance;
}


int StateMachine::process_state()
{

    std::this_thread::sleep_for(std::chrono::microseconds(500000));

	switch(current_state)
	{
		case STATE_INIT:

			break;
		case STATE_RUNNING:


			break;
		case STATE_SHUTDOWN:

			return 0;
		default:
			break;
	}

	return 1;
}

void StateMachine::set_state(char new_state)
{
	/*change the state and invoke any changes required on a state change*/
	if( current_state == new_state)
		return;


	switch(new_state)
	{
		case STATE_INIT:

			report("*STATE_INIT*\n");

			/*Instantiate the object factory*/
			pObj_Factory = new ObjectFactory();

			/*start the events process */
			pObj_Events = new Events((void*)pObj_Factory);

			/*start the renderer*/
			args_t.pIObj_Factory = pObj_Factory; // pointer to the object factory
			pObj_DrawScene = new DrawScene(&args_t);

			break;
		case STATE_RUNNING:

			report("*STATE_RUNNING*\n");

			break;
		case STATE_SHUTDOWN:

			report("*STATE_SHUTDOWN*\n");

			report("Exit initiated, start closing down the services\n");

			delete(pObj_DrawScene);
			delete(pObj_Events);
			delete(pObj_Factory);

			report("================\nApplication Finished!\n");

			break;
		default:
			return;
	}

	current_state = new_state;


}



