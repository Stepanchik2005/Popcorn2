#pragma once

#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor &color,int pen_size);
	AColor(const AColor &pen_color, int pen_size, const AColor &brush_color);
	AColor(unsigned char r, unsigned char g, unsigned char b,int pen_size);

	int Get_RGB() const;
	void Select(HDC hdc) const;
	HBRUSH Get_Brush() const;
	HPEN Get_Pen() const;
	void Set_Brush(HBRUSH brush);
	void Set_Pen(HPEN pen);
	void Select_Pen(HDC hdc) const;
	
	unsigned char R, G, B;
private:
	HPEN Pen;
	HBRUSH Brush;
	
};
//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static int Rand(int range);
	static void Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius = 2);
	static void Throw();

	static bool Level_Has_Floor;
	static int Current_Timer_Tick;

	static const AColor BG_Color, Red_Color, Blue_Color, White_Color, Letter_Color, Paraschute_Color;
	static const AColor Teleport_Color, Advertisement_Blue_Table_Color, Unbreakable_Blue_Hightlight_Color, Unbreakable_Red_Hightlight_Color;;

	static HWND Hwnd;

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
	static const double Moving_Size_Step;
};
//------------------------------------------------------------------------------------------------------------
