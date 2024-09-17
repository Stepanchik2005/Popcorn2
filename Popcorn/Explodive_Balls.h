#pragma once
#include "Config.h"

enum class EExplodive_Ball_State: unsigned char
{
	Idle,
	Charging,
	Expanding,
	Fading
};

class AExplodive_Ball: public AGraphics_Objects
{
public:
	AExplodive_Ball();

	virtual void Act();
	virtual bool Is_Finished() ;
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Explode(int x, int y, int time_offset, int max_size, int step_count, bool is_red);
	void Draw_Expanding(HDC hdc);
	void Draw_Fading(HDC hdc);

	void Redraw_Ball();
		
	//static void Setup_Colors();
	int Start_Expanding_Tick;
	EExplodive_Ball_State Explodive_Ball_State;
private:
	int X_Pos,Y_Pos;
	int Max_Size, Size;
	int Step;
	int Step_Count;
	int Current_Step;

	int Start_Fading_Tick;
	bool Is_Red;

	static const int Max_Fade_Step = AsConfig::FPS;
	static const int Fading_Timeout = 30;

	static AColor_Fade Fading_Red_Colors;
	static AColor_Fade Fading_Blue_Colors;

	
	RECT Exploding_Ball_Rect;
};
	
	
