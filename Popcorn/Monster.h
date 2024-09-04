#pragma once
#include "Explodive_Balls.h"
#include "Level.h"

enum class EEye_State: unsigned char
{
	Closed,
	Opening,
	Staring,
	Closing
};

enum class EMonster_State: unsigned char
{
	Idle,
	Emitting, 
	Alive,
	Destroying
};

class AMonster: public AGame_Object
{
public:
	AMonster();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object* ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);

	void Activate(double x_pos, double y_pos, bool is_left_gate);
	void Destroy();
	
	const int Emitting_Timeout = AsConfig::FPS * 2;
	int Time_Of_Emitting;

protected:
	virtual void Act_For_Alive_State() = 0;
	virtual void Draw_For_Alive_State(HDC hdc, RECT &paint_area) = 0;

	RECT Monster_Rect, Prev_Monster_Rect;
	int Start_Animation;
	EMonster_State Monster_State;
	static const int Explodive_Balls_Count = 10; 
	AExplodive_Ball Explodive_Balls[Explodive_Balls_Count];
	double X_Pos, Y_Pos;
		double Speed;
	static const int Width = 16;
	static const int Height = 16;
private:
	void Act_For_Destroying_State();
	void Draw_For_Destroying_State(HDC hdc, RECT &paint_area);
	void Get_Monster_Rect(double x_pos, double y_pos, RECT &monster_rect);
	void Redraw_Monster();
	void On_Change_Direction();

	int Next_Direction_Switch_Tick;

	double Direction;
	
	
};




class AMonster_Eye: public AMonster
{
public:
	AMonster_Eye();
	
private:
	 virtual void Act_For_Alive_State();
	 virtual void Draw_For_Alive_State(HDC hdc, RECT &paint_area);

	 void Find_Current_Tick(int &number_tick,  double &curr_tick,  int &prev_blick_tick);
	 void Correct_Drawing_Rect(RECT &rect, int left_offset, int top_offset, int right_offset, int bottom_offset);
	
	EEye_State Eye_State;
	double Cornea_Height;

	static const int Max_Cornea_Height = 11;
	static const int Blick_Count = 7;
	static const EEye_State Blick_State[Blick_Count];  
	static const double Blick_Tick[Blick_Count];
};

class AMonster_Comet: public AMonster
{
public:
	AMonster_Comet();

private:
	virtual void Act_For_Alive_State();
	virtual void Draw_For_Alive_State(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	int Ticks_Per_Rotation;
	double Current_Angle;

	static const int Min_Ticks_Per_Rotation = AsConfig::FPS * 2;
	static const int Max_Ticks_Per_Rotation = AsConfig::FPS * 4;
};