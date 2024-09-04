#pragma once

#include "Laser_Beam.h"

//------------------------------------
class AsLaser_Beam_Set: public AsGame_Objects_Set
{  
public:

	void Init();
	void Fire(double left_gun_x_pos, double right_gun_x_pos);
	void Disable_All();

private:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **obj);

	static const int Max_Count_Beams = 10;

	ALaser_Beam Beams[Max_Count_Beams];

};
