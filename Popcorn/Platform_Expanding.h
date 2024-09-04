#pragma once
#include "Platform_State.h"

class AsPlatform_Expanding
{
public:
	~AsPlatform_Expanding();

	AsPlatform_Expanding(AsPlatform_State &platform_state);

	void Init(AColor &inner_color, AColor &circle_color, AColor &highlight_color);
	bool Act_For_Expanding_State(double &x_pos, bool &is_correct, EPlatform_State &next_state);
	void Draw_Expanding_State(HDC hdc, double x);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Reset();

	double Expanding_Platform_Width;
private:
	void Draw_Expanding_Ball(HDC hdc, double x, bool is_left);
	void Draw_Expanding_Truss(HDC hdc, RECT &inner_rect, bool is_left);

	
	AsPlatform_State *Platform_State;
	
	AColor *Inner_Color, *Circle_Color, *Highlight_Color; // UNO Use, Not Own
	AColor *Truss_Color;
	static const double Max_Expanding_Size, Min_Expanding_Size, Expanding_Platform_Step;
	static const int Expanding_Platform_Inner_Width = 12; // игровые пиксели
};
