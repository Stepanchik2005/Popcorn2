#pragma once

#include "Tools.h"

//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static void Throw();

	static bool Level_Has_Floor;
	static int Current_Timer_Tick;

	static const AColor BG_Color, Red_Color, Blue_Color, White_Color, Monster_Dark_Red_Color,  Letter_Color, Laser_Color,Paraschute_Color;
	static const AColor Teleport_Color, Advertisement_Blue_Table_Color, Unbreakable_Blue_Hightlight_Color, Unbreakable_Red_Hightlight_Color;;
	static const AColor Cornea_Color, Iris_Color, Cornea_Arc_Color, Explodive_Red_Color, Explodive_Blue_Color;

	static HWND Hwnd;

	static const double Moving_Size_Step;
	static const double D_Global_Scale;
	static const double Accelerate_Ball_Speed;
	static const double Initial_Ball_Speed;
	static const double Min_Ball_Angle;

	static const int Global_Scale = 3;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;
	static const int Level_Width = 12;  // Ширина уровня в ячейках
	static const int Level_Height = 14;  // Высота уровня в ячейках
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
	static const int Max_X_Pos = Level_X_Offset + Cell_Width * Level_Width;
	static const int Max_Y_Pos = 199;
	static const int Platform_Y_Pos = 185;
	static const int FPS = 20;
	static const int Max_Active_Brick_Size = 10;
	static const int Max_Falling_Letters_Size = 10;
	static const int Max_Random_Number_For_Letter = 1; // вероятность	1 / Max_Random_Number_For_Letter выпадения буквы
	static const int Max_Balls_Count = 15;
	static const int Max_Movers_Count = 10;
	static const int Max_Life_Count = 12;
	static const int Floor_Y_Pos = AsConfig::Max_Y_Pos - 1;
	static const int Max_Modules_Count = 10;

	// константы платформы
	static const int Platform_Height = 7;
	static const int Platform_Normal_Width = 28;
	static const int Platform_Circle_Size = 7; // диаметр
	static const int Platform_Normal_Inner_Width = Platform_Normal_Width - Platform_Circle_Size;

	static const int Gate_Width = 6;
	static const int Gates_Count = 8;
};
//------------------------------------------------------------------------------------------------------------
class AsCommon
{
public:
	static int  Rand(int range);
	static void Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius = 2);
	static void Rect(HDC hdc, int x, int y, int width, int height, AColor color);
	static void Rect(HDC hdc, RECT &rect, AColor color);
	static void Ellipse(HDC hdc, RECT &rect, AColor color);

	static void Invalidate_Rect(RECT &rect);

	static unsigned char Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step, int max_fade_step);
	static void Get_Fading_Color(const AColor &color, int step, int max_fade_step, AColor &result_color);
};
