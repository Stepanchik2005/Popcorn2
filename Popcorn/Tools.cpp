#include"Tools.h"

//AHit_Checker
bool AHit_Checker::Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x)
{// Проверяет пересечение горизонтального отрезка (проходящего от left_x до right_x через y) с окружностью радиусом radius

	double min_x, max_x;

	// x * x + y * y = R * R
	// x = sqrt(R * R - y * y)
	// y = sqrt(R * R - x * x)

	if (y > radius)
		return false;

	x = sqrt(radius * radius - y * y);

	max_x = next_x_pos + x;
	min_x = next_x_pos - x;

	if (max_x >= left_x && max_x <= right_x  ||  min_x >= left_x && min_x <= right_x)
		return true;
	else
		return false;
}

bool AHit_Checker::Check_Hit(double next_x_pos, double next_y_pos)
{
	// реализация при перегрузке в классе ALevel
	return true;
}
bool AHit_Checker::Check_Hit(RECT& rect)
{
	// реализация при перегрузке в классе AMonster
	return false;
}
//AGraphics_Objects
AGraphics_Objects::~AGraphics_Objects()
{
}




// AMover
AMover::~AMover()
{
}


////// AColor
//AColor::~AColor()
//{
//	//if(Pen != 0)
//		DeleteObject(Pen);
//
//	//if(Brush != 0)
//		DeleteObject(Brush);
//}
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
//------------------------------------------------------------------------------------------------------------
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



//AsGame_Objects_Set-----------------------------------------------------------------------------------------------------------
void AsGame_Objects_Set::Advance(double max_speed)
{
	int index = 0;
	AGame_Object* object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		object->Advance(max_speed);
}
double AsGame_Objects_Set::Get_Speed()
{
	double max_speed = 0.0;
	int index = 0;
	AGame_Object* object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		if(object->Get_Speed() > max_speed)
			max_speed = object->Get_Speed();
		
	return max_speed;
}
void AsGame_Objects_Set::Start_Movement()
{
	int index = 0;
	AGame_Object* object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		object->Start_Movement();

}
void AsGame_Objects_Set::End_Movement()
{
   int index = 0;
	AGame_Object *object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		object->End_Movement();

}

void AsGame_Objects_Set::Act()
{
	int index = 0;
	AGame_Object *object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		object->Act();
   
}
bool AsGame_Objects_Set::Is_Finished()
{
   return false; // заглушка
}
void AsGame_Objects_Set::Draw(HDC hdc, RECT &paint_area)
{
	int index = 0;
	AGame_Object *object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		object->Draw(hdc, paint_area);
}
void AsGame_Objects_Set::Clear(HDC hdc, RECT &paint_area)
{
   int index = 0;
	AGame_Object* object = nullptr;

	while(Get_Next_Game_Object(index, &object) )
		object->Clear(hdc, paint_area);
		
}



