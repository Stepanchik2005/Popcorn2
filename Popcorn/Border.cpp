#include "Border.h"

// AsBorder
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
{
  Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
  Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
  Floor_Rect.right = AsConfig::Max_X_Pos * AsConfig::Global_Scale;
  Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;
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
}
void AsBorder::Clear(HDC hdc, RECT &paint_area)
{
	if(AsConfig::Level_Has_Floor)
	{
		AsConfig::BG_Color.Select(hdc);
		Rectangle(hdc, Floor_Rect.left, Floor_Rect.top, Floor_Rect.right, Floor_Rect.bottom);
	}
}
void AsBorder::Redraw_Floor()
{
	InvalidateRect(AsConfig::Hwnd, &Floor_Rect, FALSE);
}

//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{// Рисует элемент рамки уровня
	RECT intersection_rect, rect;

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