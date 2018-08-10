/*
 * WorldMovement.h
 *
 *  Created on: Mar 19, 2013
 *      Author: robert
 */

#ifndef WORLDMOVEMENT_H_
#define WORLDMOVEMENT_H_

typedef struct _world_move_coords {

	float m_rot_y, m_rot_x, m_rot_z;
	float k_pos_x, k_pos_y, k_pos_z;

} world_move_coords;

typedef struct _triangleCoordStruct
{
	float x1, x2, x3;
	float y1, y2, y3;
	float z1, z2, z3;

} triangleCoordStruct;

class WorldMovement {
public:
	WorldMovement();
	~WorldMovement();



protected:

	void update();
	bool location_ref_triangle();
	void terrain_collision_detect();
	void bouning_box_coordinate_update();
	void apply_gravity();
	void angle_of_climb();

	void set_height(float);
	void move_forward();
	void move_backwards();
	void turn_left();
	void turn_right();
	void look_up();
	void look_down();

	world_move_coords coords;
	triangleCoordStruct face;
	void* pObjectFactory;



};

#endif /* WORLDMOVEMENT_H_ */
