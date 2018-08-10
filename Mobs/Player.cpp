/*
 * Player.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: robert
 *
 *      This derived class handles the attributes, models
 *      and behavior of the player controlled unit
 */

#include "../Tools/debug_output.h"

#include "Player.h"

#include <thread>
#include <chrono>


Player::Player(MobParamaters mobParms) : MobArchTypeClass(mobParms)
{
	pObjectFactory = mobStats.pObjF;
	autorun_toggle = false;

}

Player::~Player()
{

}

void Player::left()
{
	turn_left();
}

void Player::right()
{
	turn_right();
}

void Player::up()
{
	look_up();
}

void Player::down()
{
	look_down();
}

void Player::forward()
{
	move_forward();
}

void Player::backward()
{
	move_backwards();
}

void Player::autorun(bool state)
{
	autorun_toggle = state;
}

void Player::jump()
{

}


/* This is ran from its own thread
 * The threadiness is handled from the base class*/
void Player::ProcessFunction()
{
	/*TEST THREAD*/
//	int x=0;
	while (areWeBeingDestroyed == false)
	{
		//report("%s thread output = %d\n", mobStats.objectName, x++);



		if(autorun_toggle)
		{
			move_forward();
		}


		if ( location_ref_triangle() )
		{
			terrain_collision_detect();
		}

        std::this_thread::sleep_for(std::chrono::microseconds(20000));
	}

}


