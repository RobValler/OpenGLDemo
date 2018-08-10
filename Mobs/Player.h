/*
 * Player.h
 *
 *  Created on: Dec 18, 2012
 *      Author: robert
 */

#ifndef PLAYER_H_
#define PLAYER_H_


#include "MobArchTypeClass.h"


class Player: public MobArchTypeClass {
public:
	Player(MobParamaters mobParms);
	~Player();

	void left();
	void right();
	void up();
	void down();
	void forward();
	void backward();
	void autorun(bool);
	void jump();

protected:

	void ProcessFunction();

	bool autorun_toggle;

};

#endif /* PLAYER_H_ */
