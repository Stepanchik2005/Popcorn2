#pragma once
#include "Laser_Beam_Set.h"
#include "Platform_State.h"

class AsPlatform_Laser
{
public:
	~AsPlatform_Laser();

	AsPlatform_Laser(AsPlatform_State &platform_state, AsLaser_Beam_Set *laser_beam_set);

	void Init(AsLaser_Beam_Set *laser_beam_set, AColor &inner_color, AColor &circle_color,  AColor &highlight_color);
	bool Act_For_Laser_State(EPlatform_State &next_state, double x_pos);
	void Draw_Laser_State(HDC hdc, double x ,RECT &platform_rect); 
	void Reset();
	void Fire(bool is_enable_firing, double x);

private:
	void Draw_Inner_Part(HDC hdc, double x);
	void Draw_Laser_Wing(HDC hdc, double x_pos, bool is_left);
	void Draw_Laser_Leg(HDC hdc, double x_pos, bool is_left);
	void Draw_Laser_Cabin(HDC hdc, double x_pos);
	void Draw_Expanding_Figure(HDC hdc, EFigure_Type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height);
	double Get_Expanding_Value(HDC hdc, double start_pos, double end_pos, double ratio);
	double Get_Gun_Pos(bool is_left, double platform_x_pos);
	AsPlatform_State *Platform_State;

	AColor *Inner_Color, *Circle_Color; // UNO
	AColor *Gun_Color;
	AsLaser_Beam_Set *Laser_Beam_Set; // UNO
	int Laser_Transformation_Step;
	int Last_Laser_Timer_Shot;	
	bool Enable_Laser_Firing;

	static const int Max_Laser_Transformation_Step = 20;
	static const int Laser_Shot_Timeout = AsConfig::FPS / 2;
};
