#include "Mop_Indicator.h"
//AMop_Indicator------------------------------------------------------------------------------------
AColor_Fade AMop_Indicator::Fading_Blue_Colors;

AMop_Indicator::AMop_Indicator(int x_pos, int y_pos, int time_offset)
	: X_Pos(x_pos), Y_Pos(y_pos), Time_Offset(time_offset), Current_Color(&AsConfig::Red_Color)
{
	Set_Y_Pos(y_pos); 
}
//------------------------------------------------------------------------------------
AMop_Indicator::AMop_Indicator(int x_pos, int y_pos, const AColor& dest_color)
	: X_Pos(x_pos), Y_Pos(y_pos), Current_Color(&dest_color)
{
	const int scale = AsConfig::Global_Scale;

	Mop_Indicator_Rect.left = X_Pos * scale;
	Mop_Indicator_Rect.top = Y_Pos * scale;
	Mop_Indicator_Rect.right = (X_Pos + Width) * scale;
	Mop_Indicator_Rect.bottom = (Y_Pos + Height) * scale;
	
}
//------------------------------------------------------------------------------------
void AMop_Indicator::Act()
{
	int total_timeout = Normal_Timeout + Max_Fade_Step;
	int current_offset;
	int curr_tick = (AsConfig::Current_Timer_Tick + Time_Offset) % total_timeout;

	if(curr_tick < Normal_Timeout)
		Current_Color = &AsConfig::Red_Color;
	else
	{
		current_offset = curr_tick - Normal_Timeout;

		if(current_offset < 0 || current_offset >= Max_Fade_Step)
			AsConfig::Throw();

		Current_Color = Fading_Blue_Colors.Get_Color(current_offset);
	}
	AsCommon::Invalidate_Rect(Mop_Indicator_Rect);
}
//------------------------------------------------------------------------------------
bool AMop_Indicator::Is_Finished()
{
	return false;
}
void AMop_Indicator::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	const int scale = AsConfig::Global_Scale;

	if(IntersectRect(&intersection_rect, &paint_area, &Mop_Indicator_Rect))
	{	
		AsCommon::Rect(hdc, X_Pos, Y_Pos, Width, Height, *Current_Color);

		AsConfig::Highlight_Color.Select(hdc);
		MoveToEx(hdc, X_Pos * scale, (Y_Pos + Height) * scale, 0); 
		LineTo(hdc, X_Pos * scale, Y_Pos * scale); 
		LineTo(hdc, (X_Pos + Width) * scale, Y_Pos * scale); 

		AsConfig::Shadow_Color.Select(hdc);
		MoveToEx(hdc, (X_Pos + Width) * scale, Y_Pos * scale, 0);
		LineTo(hdc, (X_Pos + Width) * scale, (Y_Pos + Height) * scale);
		LineTo(hdc, X_Pos * scale, (Y_Pos + Height) * scale);
	}
	
}
//------------------------------------------------------------------------------------
void AMop_Indicator::Clear(HDC hdc, RECT &paint_area)
{
}
//------------------------------------------------------------------------------------
void AMop_Indicator::Set_Y_Pos(int y_pos)
{
	const int scale = AsConfig::Global_Scale;

	Y_Pos = y_pos; 
	
	Mop_Indicator_Rect.left = X_Pos * scale;
	Mop_Indicator_Rect.top = Y_Pos * scale;
	Mop_Indicator_Rect.right = (X_Pos + Width) * scale;
	Mop_Indicator_Rect.bottom = (Y_Pos + Height) * scale;
}
//------------------------------------------------------------------------------------
void AMop_Indicator::Init()
{
  AMop_Indicator::Fading_Blue_Colors.Init(AsConfig::Red_Color, AsConfig::Blue_Color, Max_Fade_Step); // используем метод отложенной инициализации, т.к. цвета в конфиге инициализируются позже,
																																	  // чем AColor_Fade AMop_Indicator::Fading_Blue_Colors;
}