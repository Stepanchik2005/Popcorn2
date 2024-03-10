#pragma once

#include "Ball_Set.h"
#include "Falling_Letter.h"
//------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
enum EPlatform_State
{
	EPS_Missing,
	EPS_Ready,
	EPS_Normal,
	EPS_Pre_Meltdown,
	EPS_Meltdown,
	EPS_Roll_In,
	EPS_Expand_Roll_In,
	EPS_Glay_Init,
	EPS_Glay,
	EPS_Glay_Finalize
};
enum EPlatform_Moving_State
{
	EPMS_Stopping,
	EPMS_Stop,
	EPMS_Left,
	EPMS_Right
};
//------------------------------------------------------------------------------------------------------------
class AsPlatform: public AHit_Checker, public AMover, public AGraphics_Objects
{
public:
	~AsPlatform();

	AsPlatform();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Init(AsBall_Set *ball_set);
	void On_Space_Key(bool key_down);
	EPlatform_State Get_State();
	void Set_State(EPlatform_State new_state);
	void Redraw_Platform(bool update_rect = true);
   void Move(bool is_left, bool key_down);
	bool HitBy(AFalling_Letter *falling_letter);
	double Get_Middle_Pos();
	

	
	int Width;
	double X_Pos;

private:
	void Clear_BG(HDC hdc, RECT &paint_area);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Draw_Normal_State(HDC hdc, RECT &paint_area);
	void Draw_Glay_State(HDC hdc, RECT &paint_area);
	void Draw_Spot_State(HDC hdc, int x_offset, int width, int height);
	void Get_Normal_Platform_Image(HDC hdc);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);
	void Draw_Roll_In_State(HDC hdc, RECT &paint_area);
	void Draw_Expanding_Roll_In_State(HDC hdc, RECT &paint_area);
	bool Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball);
	bool Get_Platform_Image_Stroke_Color(int x, int y, AColor &color, int &stroke_length);

	EPlatform_State Platform_State;
	EPlatform_Moving_State Platform_Moving_State;
	int Inner_Width;
	int Rolling_Step;
	int Normal_Platform_Image_Width;
	int Normal_Platform_Image_Height;
	bool Left_Key_Dowm;
	bool Right_Key_Down;
	double Speed;
	double Platform_Glay_Ratio;
	int *Normal_Platform_Image; 

	static const int Normal_Width = 28;

	int Meltdown_Platform_Y_Pos[Normal_Width * AsConfig::Global_Scale];

	RECT Platform_Rect, Prev_Platform_Rect;

   AColor Highlight_Pen_Color, Platform_Circle_Color, Platform_Inner_Color;
	AsBall_Set *Ball_Set;
	
	static const int Height = 7;
	static const int Circle_Size = 7; // диаметр
	static const int Normal_Platform_Inner_Width = Normal_Width - Circle_Size;
	static const int Meltdown_Speed = 3;
	static const int Max_Rolling_Step = 16;
	static const int Roll_In_Platform_End_X_Pos = 99;
	static const int Rolling_Platform_Speed = 3;
	static const int X_Step = 6;
	static const double Max_Glay_Ratio, Min_Glay_Ratio;
};
//------------------------------------------------------------------------------------------------------------
