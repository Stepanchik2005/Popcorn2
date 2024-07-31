#pragma once
#include "Ball.h"

class AsBall_Set : public AsGame_Objects_Set
{
public:
	virtual void Act();
	
	void Release_From_Platform(double ball_mid_pos);
	bool Release_Next_Ball();
	void On_Platform_Advance(double direction, double speed, double max_speed);
	void Set_On_Platform(double ball_mid_pos);
	int  Count_Lost_Ball();
	void Set_For_Test();
	bool Is_Test_Finished();
	void Triple_Balls();
	void Inverse_Balls();
	void Accelerate();
	void Reset_Speed();

	ABall Balls[AsConfig::Max_Balls_Count];
private:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **obj);
};