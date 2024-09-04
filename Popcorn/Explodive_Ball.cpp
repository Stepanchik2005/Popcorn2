#include "Explodive_Balls.h"

//AExplodiveBalls---------------------------------------------------------
AColor AExplodive_Ball::Fading_Red_Colors[Max_Fade_Step];
AColor AExplodive_Ball::Fading_Blue_Colors[Max_Fade_Step];

AExplodive_Ball::AExplodive_Ball()
	: X_Pos(0),Y_Pos(0), Max_Size(0), Size(0), Step_Count(0), Step(0), Start_Expanding_Tick(0), Current_Step(0), Is_Red(true),
	  Start_Fading_Tick(0),Explodive_Ball_State(EExplodive_Ball_State::Idle), Exploding_Ball_Rect{}  
{
}
void AExplodive_Ball::Act()
{	
	double ratio;

	switch(Explodive_Ball_State)
	{
	case EExplodive_Ball_State::Idle:
		return;
	
	case EExplodive_Ball_State::Charging:
		if(AsConfig::Current_Timer_Tick >= Start_Expanding_Tick)
			Explodive_Ball_State = EExplodive_Ball_State::Expanding;
		break;

	case EExplodive_Ball_State::Expanding:
		Current_Step += Step;
		ratio = 1.0 - (double)(Max_Size - Current_Step) / (double)Max_Size;

		Size =  (int)((double)Max_Size * ratio);

		if(Size > Max_Size)
		{
			Explodive_Ball_State = EExplodive_Ball_State::Fading;
			Start_Fading_Tick = AsConfig::Current_Timer_Tick;
		}

		break;

	case EExplodive_Ball_State::Fading:
		if(AsConfig::Current_Timer_Tick > Start_Fading_Tick + Fading_Timeout)
			Explodive_Ball_State = EExplodive_Ball_State::Idle;
		break;

	}
	Redraw_Ball();
}
bool AExplodive_Ball::Is_Finished()
{
	return false;	
}
void AExplodive_Ball::Draw(HDC hdc, RECT &paint_area)
{
	
	switch(Explodive_Ball_State)
	{
	case EExplodive_Ball_State::Idle:
	case EExplodive_Ball_State::Charging:
		break;

	case EExplodive_Ball_State::Expanding:
		Draw_Expanding(hdc);
		break;

	case EExplodive_Ball_State::Fading:
		Draw_Fading(hdc);
		break;
	}
}

void AExplodive_Ball::Clear(HDC hdc, RECT &paint_area)
{
	// заглушка
}

void AExplodive_Ball::Explode(int x, int y, int time_offset, int max_size, int step_count, bool is_red)
{

	X_Pos = x; // координаты ЦЕНТРА МЯЧИКА
	Y_Pos = y;
	Max_Size = max_size;
	Step_Count = step_count;
	Step = Max_Size / Step_Count;

	Start_Expanding_Tick = AsConfig::Current_Timer_Tick + time_offset;

	Explodive_Ball_State = EExplodive_Ball_State::Charging;

	Is_Red = is_red;
}
void AExplodive_Ball::Draw_Expanding(HDC hdc)
{
	const AColor *color;
	const int scale = AsConfig::Global_Scale;

	if(Is_Red)
		color = &AsConfig::Explodive_Red_Color;
	else
		color = &AsConfig::Explodive_Blue_Color;

	AsCommon::Ellipse(hdc, Exploding_Ball_Rect, *color);
}
void AExplodive_Ball::Draw_Fading(HDC hdc)
{
	int curr_timeout = 0;
	double ratio;
	int color_index;
	AColor *color;
	curr_timeout = AsConfig::Current_Timer_Tick - Start_Fading_Tick;

	if(curr_timeout > Fading_Timeout)
		curr_timeout = Fading_Timeout;

	ratio = (double)curr_timeout / (double)Fading_Timeout;

	color_index = (int)round(ratio * (double)(Max_Fade_Step - 1) ) ;

	if(Is_Red)
		color = &Fading_Red_Colors[color_index];
	else
		color = &Fading_Blue_Colors[color_index];

	AsCommon::Ellipse(hdc, Exploding_Ball_Rect, *color);
}

void AExplodive_Ball::Setup_Colors()
{
	int i;

	for (i = 0; i < Max_Fade_Step; i++)
	{
		AsCommon::Get_Fading_Color(AsConfig::Red_Color, i, Max_Fade_Step, Fading_Red_Colors[i]);
		AsCommon::Get_Fading_Color(AsConfig::Blue_Color, i, Max_Fade_Step, Fading_Blue_Colors[i]);
	}
}
void AExplodive_Ball::Redraw_Ball()
{
	Exploding_Ball_Rect.left = (int)((double)X_Pos - (double)Size / 2.0);
	Exploding_Ball_Rect.top = (int)((double)Y_Pos - (double)Size / 2.0) ;
	Exploding_Ball_Rect.right = Exploding_Ball_Rect.left + Size;
	Exploding_Ball_Rect.bottom = Exploding_Ball_Rect.top + Size;

	AsCommon::Invalidate_Rect(Exploding_Ball_Rect);
}