#include "Config.h"

// AsConfig
bool AsConfig::Level_Has_Floor = true;
int AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(15, 63, 31);
const AColor AsConfig::Red_Color(255, 85, 85);
const AColor AsConfig::Blue_Color(85, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Monster_Dark_Red_Color(200, 0, 32);

const AColor AsConfig::Letter_Color(AsConfig::White_Color, AsConfig::Global_Scale);
const AColor AsConfig::Laser_Color(AsConfig::White_Color, AsConfig::Global_Scale);
const AColor AsConfig::Paraschute_Color(AsConfig::Blue_Color, 3, AsConfig::Red_Color);
const AColor AsConfig::Teleport_Color(AsConfig::Blue_Color, 3, AsConfig::BG_Color);
const AColor AsConfig::Advertisement_Blue_Table_Color(0, 159, 159, AsConfig::Global_Scale);
const AColor AsConfig::Unbreakable_Blue_Hightlight_Color(AsConfig::Blue_Color, AsConfig::Global_Scale); 
const AColor AsConfig::Unbreakable_Red_Hightlight_Color(AsConfig::Red_Color, 2 * AsConfig::Global_Scale);
const AColor AsConfig::Cornea_Color(AsConfig::BG_Color, AsConfig::Global_Scale - 1, AsConfig::White_Color);
const AColor AsConfig::Iris_Color(AsConfig::BG_Color, AsConfig::Global_Scale * 2 / 3, AsConfig::Blue_Color);
const AColor AsConfig::Cornea_Arc_Color(AsConfig::BG_Color, AsConfig::Global_Scale * 2 / 3);


HWND AsConfig::Hwnd;

const double AsConfig::Moving_Size_Step = 1.0 / AsConfig::Global_Scale;
const double AsConfig::D_Global_Scale = 3.0;
const double AsConfig::Accelerate_Ball_Speed = 1.001;
const double AsConfig::Initial_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;

//------------------------------------------------------------------------------------------------------------
void AsConfig::Throw()
{
	throw 13;
}



// AsCommon --------
int AsCommon::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / RAND_MAX;
}
//------------------------------------------------------------------------------------------------------------
void AsCommon::Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius)
{
	int radius = corner_radius * AsConfig::Global_Scale;

	RoundRect(hdc, brick_rect.left, brick_rect.top, brick_rect.right - 1, brick_rect.bottom - 1, radius, radius);
}
void AsCommon::Rect(HDC hdc, int x, int y, int width, int height, AColor color)
{
	RECT rect = {};
	const int scale = AsConfig::Global_Scale;
	color.Select(hdc);

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = rect.left + width * scale;
	rect.bottom = rect.top + height * scale;

	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Rect(HDC hdc, RECT &rect, AColor color)
{
	color.Select(hdc);

	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Ellipse(HDC hdc, RECT &rect, AColor color)
{
	color.Select(hdc);

	::Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Invalidate_Rect(RECT &rect)
{
	InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
}