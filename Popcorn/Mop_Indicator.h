#pragma once
#include "Config.h"
//------------------------------------------------------------------------------------------------------------
class AMop_Indicator: public AGraphics_Objects
{
public:
	AMop_Indicator(int x_pos, int y_pos, int time_offset);
	AMop_Indicator(int x_pos, int y_pos, const AColor& dest_color);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Set_Y_Pos(int y_pos);

	static void Init();

private:
	int X_Pos, Y_Pos;
	RECT Mop_Indicator_Rect;
	const AColor *Current_Color;
	int Time_Offset;


	static AColor_Fade Fading_Blue_Colors;
	static const int Width = 17;
	static const int Height = 5;
	static const int Max_Fade_Step = AsConfig::FPS * 4 / 10;
	static const int Normal_Timeout = AsConfig::FPS;

	
};
