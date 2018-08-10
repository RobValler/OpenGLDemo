/*
 * state_machine.h
 *
 *  Created on: Sep 1, 2013
 *      Author: robert
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

enum state_machine_states
{
	STATE_UNKNOWN,
	STATE_INIT,
	STATE_RUNNING,
	STATE_SHUTDOWN,
};


class StateMachine
{
    public:
        static StateMachine& getInstance();
        static void setParameters(int, char**);
    	static int process_state();
    	static void set_state(char);


    private:
        StateMachine() {};
        StateMachine(StateMachine const&);
        void operator=(StateMachine const&);



};

#endif /* STATE_MACHINE_H_ */
