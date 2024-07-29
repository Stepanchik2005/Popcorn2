#include "Border.h"




AGate::AGate(int x_pos, int y_pos)
	: X_Pos(x_pos), Y_Pos(y_pos), Height(19)
{
}
//------------------------------------------------------------------------------------------------------------
void AGate::Act()
{
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw(HDC hdc, RECT &paint_area) 
{
	RECT intersection_rect = {}, rect = {};
	const int scale = AsConfig::Global_Scale;

	rect.left = X_Pos * scale;
	rect.top = Y_Pos * scale;
	rect.right = rect.left + AsConfig::Gate_Width * scale;
	rect.bottom = rect.top + Height * scale;

	if(! IntersectRect(&intersection_rect, &paint_area, &rect) )
		return;

	Draw_Cup(hdc, paint_area, true); // отрисовка верхней чаши гейта
	Draw_Cup(hdc, paint_area, false);// отрисовка нижней чаши гейта
	
	// 1.6 Рисование перемычек между двумя чашами
	Draw_Edges(hdc, paint_area);
}


//------------------------------------------------------------------------------------------------------------
void AGate::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect = {}, rect = {};
	const int scale = AsConfig::Global_Scale;

	rect.left = X_Pos * scale;
	rect.top = Y_Pos * scale;
	rect.right = rect.left + AsConfig::Gate_Width * scale;
	rect.bottom = rect.top + Height * scale;

	if(! IntersectRect(&intersection_rect, &paint_area, &rect) )
		return;

	AsConfig::Rect(hdc, rect, AsConfig::BG_Color);

}
void AGate::Draw_Cup(HDC hdc, RECT &paint_area, bool is_top_cup) 
{
	RECT rect = {}, intersection_rect = {};
	HRGN region;
	XFORM xform = {}, old_xform = {};
	const int scale = AsConfig::Global_Scale;
	const int half_scale = AsConfig::Global_Scale / 2;
	int x = 0, y = 0;
	int cup_offset;
	
	xform.eM11 = 1.0f;
	xform.eM12 = 0.0f;
	xform.eM21 = 0.0f;
	xform.eDx = (float)X_Pos * scale;
	

	if(is_top_cup)
	{
		xform.eM22 = 1.0f;
		cup_offset = 0;
	}
	else
	{
		xform.eM22 = -1.0f;
		cup_offset = 18 * scale + 2;
	}

	xform.eDy = (float)Y_Pos * scale + cup_offset;

	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	// 1. Рисование синего основного прямоугольника
	
	rect.left = x * scale;
	rect.top = (y + 1) * scale;
	rect.right = rect.left + 6 * scale;
	rect.bottom = rect.top + 4 * scale;

	

	AsConfig::Blue_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, rect, scale);

	// 1.1 Рисование белой каймы слева
	
	AsConfig::Letter_Color.Select_Pen(hdc);

	rect.left = X_Pos * scale;
	
	rect.right = rect.left + 3 * scale;
	rect.bottom = rect.top + 4 * scale;

	if(is_top_cup)
	{
		rect.top = (Y_Pos + 1) * scale;
		rect.bottom = rect.top + 4 * scale;
	}
	else
	{
		rect.top = (Y_Pos - 1) * scale + cup_offset + 1;
		rect.bottom = rect.top - 4 * scale;
	}
   region = CreateRectRgnIndirect(&rect);
   SelectClipRgn(hdc, region);

	rect.left = x * scale + half_scale;
	rect.top = (y + 1) * scale + half_scale;
	rect.right = rect.left + 5 * scale + half_scale;
	rect.bottom = rect.top + 5 * scale + half_scale;

	AsConfig::Round_Rect(hdc, rect, scale);

   SelectClipRgn(hdc, 0);
   DeleteObject(region);

	// 1.2 Рисование нижней белой линии 
	MoveToEx(hdc, x * scale + 1, (y + 4) * scale + 1, 0);
	LineTo(hdc, (x + 4) * scale, (y + 4) * scale + 1);

	// 1.3 Рисование "заплатки"
	AsConfig::Rect(hdc, x + 4, y + 3, 2, 2, AsConfig::Blue_Color);

	// 1.4 Рисование перфорации
	AsConfig::Rect(hdc, x + 4, y + 3, 1, 1, AsConfig::BG_Color);

	// 1.5 Рисование перемычки над/снизу чаши
	AsConfig::Rect(hdc, x + 2, y + 0, 2, 1, AsConfig::Blue_Color);

	SetWorldTransform(hdc, &old_xform);

	
}
void AGate::Draw_Edges(HDC hdc, RECT &paint_area) 
{
	int i;
	int y_offset = 0;
	int y = Y_Pos + 5;
	bool is_top_cup = true;
	for(i = 0; i < AGate::Gates_Jumper_Count; ++i) // отрисовка перемычек между двумя чашами
	{
		Draw_Edge(hdc, paint_area, X_Pos, y + y_offset, is_top_cup);

		is_top_cup = !is_top_cup;

		y_offset++;
	}
}

void AGate::Draw_Edge(HDC hdc, RECT &paint_area, int x, int y, bool is_blue) 
{
	const int scale = AsConfig::Global_Scale;

	if(is_blue)
	{
		AsConfig::Rect(hdc,x, y, 1, 1, AsConfig::BG_Color);
		AsConfig::Rect(hdc,x + 1, y, 2, 1, AsConfig::Blue_Color);
		AsConfig::Rect(hdc,x + 3, y, 1, 1, AsConfig::BG_Color);
		AsConfig::Rect(hdc,x + 4, y, 1, 1, AsConfig::Blue_Color);
		AsConfig::Rect(hdc,x + 5, y, 1, 1, AsConfig::BG_Color);
	}
	else
	{
		AsConfig::Rect(hdc,x, y, 4, 1, AsConfig::White_Color);
		AsConfig::Rect(hdc,x + 4, y, 2, 1, AsConfig::Blue_Color);
	}
}
// AsBorder

AsBorder::~AsBorder()
{
	int i; 
	for (i = 0; i < AsConfig::Gates_Count; ++i)
		delete Gate[i];
}
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
	: Floor_Rect{}, Gate{}
{
	  Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	  Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
	  Floor_Rect.right = AsConfig::Max_X_Pos * AsConfig::Global_Scale;
	  Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;

	  Gate[0] = new AGate(1, 29);
	  Gate[1] = new AGate(AsConfig::Max_X_Pos, 29);

	  Gate[2] = new AGate(1, 77);
	  Gate[3] = new AGate(AsConfig::Max_X_Pos, 77);

	  Gate[4] = new AGate(1, 129);
	  Gate[5] = new AGate(AsConfig::Max_X_Pos, 129);

	  Gate[6] = new AGate(1, 178);
	  Gate[7] = new AGate(AsConfig::Max_X_Pos, 178);

}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{	// Корректируем позицию при отражении от рамки

	bool got_hit = false;

	// Левый край
	if (next_x_pos - ball->Radius < AsConfig::Border_X_Offset)
	{
		got_hit = true;
		ball->Reflect(false);
	}
	
	// Верхний край
	if (next_y_pos - ball->Radius < AsConfig::Border_Y_Offset)
	{
		got_hit = true;
		ball->Reflect(true);
	}

	// Нижний край
	if (next_x_pos + ball->Radius > AsConfig::Max_X_Pos)
	{
		got_hit = true;
		ball->Reflect(false);
	}

	// Правый край
	if (next_y_pos + ball->Radius > AsConfig::Floor_Y_Pos && AsConfig::Level_Has_Floor)
	{
		got_hit = true;
		ball->Reflect(true);
	}
	else
	{
		if (next_y_pos + ball->Radius > (double)AsConfig::Max_Y_Pos + ball->Radius * 4.0)  // Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы
			ball->Set_State(EBS_Lost, next_x_pos);
	}
	
	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
}
bool AsBorder::Is_Finished()
{
	return false;
}
void AsBorder::Draw(HDC hdc, RECT &paint_area)
{// Рисует рамку уровня
	int i;

	// 1. Линия слева
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 2, 1 + i * 4, false);

	// 2. Линия справа
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 201, 1 + i * 4, false);

	// 3. Линия сверху
	for (i = 0; i < 50; i++)
		Draw_Element(hdc, paint_area, 3 + i * 4, 0, true);

	if(AsConfig::Level_Has_Floor)
		Draw_Floor(hdc, paint_area);

	for (i = 0; i < AsConfig::Gates_Count; i++)
		Gate[i]->Draw(hdc, paint_area);
	
	
}
void AsBorder::Clear(HDC hdc, RECT &paint_area)
{
	int i;
	if(AsConfig::Level_Has_Floor)
	{
		AsConfig::BG_Color.Select(hdc);
		Rectangle(hdc, Floor_Rect.left, Floor_Rect.top, Floor_Rect.right, Floor_Rect.bottom);
	}

	for (i = 0; i < AsConfig::Gates_Count; i++)
		Gate[i]->Clear(hdc, paint_area);
}
void AsBorder::Redraw_Floor()
{
	AsConfig::Invalidate_Rect(Floor_Rect);
}

//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{// Рисует элемент рамки уровня
	RECT intersection_rect = {}, rect = {};

	rect.left = x * AsConfig::Global_Scale;
	rect.top = y * AsConfig::Global_Scale;
	rect.right = (x + 4) * AsConfig::Global_Scale;
	rect.bottom = (y + 4) * AsConfig::Global_Scale;

	if(! IntersectRect(&intersection_rect, &paint_area, &rect) )
		return;
 // Основная линия
	AsConfig::Blue_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);
	else
		Rectangle(hdc, (x + 1) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

	// Белая кайма
	AsConfig::White_Color.Select(hdc);

	if (top_border)
		Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 1) * AsConfig::Global_Scale - 1);
	else
		Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

	// Перфорация
	AsConfig::BG_Color.Select(hdc);
	if (top_border)
		Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 3) * AsConfig::Global_Scale - 1);
	else
		Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 2) * AsConfig::Global_Scale - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT &paint_area)
{
	int i;
	int len_stroke, len_gap;
	int stroke_count;
	int stroke_len;
	double x_pos, y_pos;
	RECT intersection_rect;

	if(! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	len_stroke = 4 * AsConfig::Global_Scale;
	len_gap = 2 * AsConfig::Global_Scale;
	stroke_len = len_stroke + len_gap;
	stroke_count = (Floor_Rect.right - Floor_Rect.left) / stroke_len;

	x_pos = Floor_Rect.left;
	//y_pos = (Floor_Rect.bottom - Floor_Rect.top) / 2;
	y_pos = Floor_Rect.top;
	AsConfig::Letter_Color.Select(hdc);
	for(i = 0; i < stroke_count; ++i)
	{
	

		MoveToEx(hdc, x_pos, y_pos, 0);
		LineTo(hdc, x_pos + len_stroke, y_pos);

		x_pos +=  stroke_len;
	}

}