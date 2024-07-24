#include "Config.h"

// AColor
//------------------------------------------------------------------------------------------------------------
AColor::AColor()
: R(0), G(0), B(0), Pen(0), Brush(0)
{
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b)
: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, 0, RGB(r,g,b));
	Brush = CreateSolidBrush(RGB (r,g,b));
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(const AColor &color, int pen_size)
: R(color.R), G(color.G), B(color.B), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, color.Get_RGB());
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(const AColor &pen_color, int pen_size, const AColor &brush_color)
: R(0), G(0), B(0), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, pen_color.Get_RGB());
	Brush = CreateSolidBrush(brush_color.Get_RGB());
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b,int pen_size)
: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, RGB(r, g, b));
}
void AColor::Select(HDC hdc) const
{
	SelectObject(hdc, Pen);
	SelectObject(hdc, Brush); 
}
//------------------------------------------------------------------------------------------------------------
HBRUSH AColor::Get_Brush() const
{
	return Brush;
}
//------------------------------------------------------------------------------------------------------------
HPEN AColor::Get_Pen() const
{
	return Pen;
}
//------------------------------------------------------------------------------------------------------------
int AColor::Get_RGB() const
{
	return RGB(R, G, B);
}
//------------------------------------------------------------------------------------------------------------

void AColor::Set_Brush(HBRUSH brush)
{
	Brush = brush;
}
//------------------------------------------------------------------------------------------------------------

void AColor::Set_Pen(HPEN pen)
{
	Pen = pen;
}
void AColor::Select_Pen(HDC hdc) const
{
	SelectObject(hdc, Pen);
}
//------------------------------------------------------------------------------------------------------------




// AsConfig


bool AsConfig::Level_Has_Floor = true;
int AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(15, 63, 31);
const AColor AsConfig::Red_Color(255, 85, 85);
const AColor AsConfig::Blue_Color(85, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Letter_Color(AsConfig::White_Color, AsConfig::Global_Scale);
const AColor AsConfig::Laser_Color(AsConfig::White_Color, AsConfig::Global_Scale);
const AColor AsConfig::Paraschute_Color(AsConfig::Blue_Color, 3, AsConfig::Red_Color);
const AColor AsConfig::Teleport_Color(AsConfig::Blue_Color, 3, AsConfig::BG_Color);
const AColor AsConfig::Advertisement_Blue_Table_Color(0, 159, 159, AsConfig::Global_Scale);
const AColor AsConfig::Unbreakable_Blue_Hightlight_Color(AsConfig::Blue_Color, AsConfig::Global_Scale); 
const AColor AsConfig::Unbreakable_Red_Hightlight_Color(AsConfig::Red_Color, 2 * AsConfig::Global_Scale);


HWND AsConfig::Hwnd;

const double AsConfig::Moving_Size_Step = 1.0 / AsConfig::Global_Scale;
const double AsConfig::D_Global_Scale = 3.0;
const double AsConfig::Accelerate_Ball_Speed = 1.001;
const double AsConfig::Initial_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;
//------------------------------------------------------------------------------------------------------------
int AsConfig::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / RAND_MAX;
}
//------------------------------------------------------------------------------------------------------------
void AsConfig::Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius)
{
	int radius = corner_radius * AsConfig::Global_Scale;

	RoundRect(hdc, brick_rect.left, brick_rect.top, brick_rect.right - 1, brick_rect.bottom - 1, radius, radius);
}
//------------------------------------------------------------------------------------------------------------
void AsConfig::Throw()
{
	throw 13;
}
void AsConfig::Invalidate_Rect(RECT &rect)
{
	InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
}