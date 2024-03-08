#pragma once
#include "Ball.h"

class AsBall_Set : public AMover, public AGraphics_Objects
{
public:
	virtual void Advance(double max_speed);
	virtual double Get_Speed();
   virtual void Start_Movement();
   virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Release_From_Platform(double ball_mid_pos);
	void Set_On_Platform(double ball_mid_pos);
	int  Count_Lost_Ball();
	void Set_For_Test();
	bool Is_Test_Finished();
	void Triple_Balls();
	void Inverse_Balls();
	void Accelerate();
	void Reset_Speed();

	ABall Balls[AsConfig::Max_Balls_Count];
};