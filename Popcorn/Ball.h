#pragma once
#include "Config.h"
#include "Hit_Checker_List.h"



class ABall: public AGame_Object, public ABall_Object
{
public:
	ABall();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	virtual double Get_Direction();
	virtual void Set_Direction(double new_direction);
	virtual bool Is_Moving_Up();
	virtual EBall_State Get_State();
	virtual void Set_State(EBall_State new_state, double x_pos = 0, double y_pos = 0);
	virtual void Reflect(bool from_horizontal);
	virtual void Get_Center(double &x_pos, double &y_pos);
	virtual bool Is_Moving_Left();
	virtual void Set_On_Paraschute(int brick_x, int brick_y);
	virtual void Draw_Teleporting(HDC hdc, int step);

	void Set_Speed(double ball_speed);
	void Set_For_Test();
	bool Is_Test_Finished();
	void Forced_Advance(double direction, double speed, double max_speed);
	void Release();
	
	static AHit_Checker_List Hit_Checker_List;
	int Release_Timer_Tick;
	static const double Start_Ball_Y_Pos;

private:
	void Redraw_Ball();
	void Draw_Paraschute(HDC hdc, RECT &paint_area);
	void Redraw_Paraschute();
	void Clean_Up_Paraschute(HDC hdc);

	EBall_State Ball_State, Prev_Ball_State;
	double Prev_Ball_Speed;
	double Ball_Speed;

	double Prev_Ball_Direction;
	double Ball_Direction;

	bool Testing_Is_Active;
	int Test_Iteration;
	double Rest_Test_Distance;

	double Center_X_Pos, Center_Y_Pos; // игровые коориднаты (нужно умножать на масштаб)

	RECT Ball_Rect, Prev_Ball_Rect;
   RECT Paraschute_Rect, Prev_Paraschute_Rect;

	static const int Paraschute_Size = 15;
	static const int On_Platform_Timeout = 3 * AsConfig::FPS;
	
};
//------------------------------------------------------------------------------------------------------------
