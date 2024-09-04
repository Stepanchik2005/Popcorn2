#pragma once
#include "Config.h"
#include "Hit_Checker_List.h"

enum class Elaser_Beam_State
{
	Disabled,
	Active,
	Stopping,
	Cleanup
};


class ALaser_Beam: public AGame_Object
{
public:
	ALaser_Beam();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Set_At(double x_pos, double y_pos);
	bool Is_Active();
	void Disable();
	static AHit_Checker_List Hit_Checker_List;

	Elaser_Beam_State Laser_Beam_State;

	

private:
	double X_Pos, Y_Pos; 
	double Speed;
	RECT Beam_Rect, Prev_Beam_Rect;
	static const int Width = 1;
	static const int Height = 3;

	void Redraw_Beam();
};