/*
 ============================================================================
 Name        :
 Author      :
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */


#include "state_machine.h"


int main(int argc, char **argv)
{

	StateMachine::getInstance();

	StateMachine::set_state(STATE_INIT);
	StateMachine::set_state(STATE_RUNNING);

	/* MAIN PROGRAM LOOP */
	while(1)
	{
		// exit conditions
		if(StateMachine::process_state() == 0)
		{
			break;
		}

	}



	return 1;
}
