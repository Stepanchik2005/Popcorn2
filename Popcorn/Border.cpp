#include "Border.h"


//------------------------------------------------------------------------------------------------------------
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

	  Gate[0] = new AGate(1, 29, 0, 3);
	  Gate[1] = new AGate(AsConfig::Max_X_Pos, 29, AsConfig::Level_Width - 1, 3);

	  Gate[2] = new AGate(1, 77, 0, 9);
	  Gate[3] = new AGate(AsConfig::Max_X_Pos, 77, AsConfig::Level_Width - 1, 9);

	  Gate[4] = new AGate(1, 129);
	  Gate[5] = new AGate(AsConfig::Max_X_Pos, 129);

	  Gate[6] = new AGate(1, 178);
	  Gate[7] = new AGate(AsConfig::Max_X_Pos, 178);

}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{	// Корректируем позицию при отражении от рамки

	bool got_hit = false;

	// Левый край
	if (next_x_pos - AsConfig::Ball_Radius < AsConfig::Border_X_Offset)
	{
		got_hit = true;
		ball->Reflect(false);
	}
	
	// Верхний край
	if (next_y_pos - AsConfig::Ball_Radius < AsConfig::Border_Y_Offset)
	{
		got_hit = true;
		ball->Reflect(true);
	}

	// Нижний край
	if (next_x_pos + AsConfig::Ball_Radius > AsConfig::Max_X_Pos)
	{
		got_hit = true;
		ball->Reflect(false);
	}

	// Правый край
	if (next_y_pos + AsConfig::Ball_Radius > AsConfig::Floor_Y_Pos && AsConfig::Level_Has_Floor)
	{
		got_hit = true;
		ball->Reflect(true);
	}
	else
	{
		if (next_y_pos + AsConfig::Ball_Radius > (double)AsConfig::Max_Y_Pos + AsConfig::Ball_Radius * 4.0)  // Чтобы шарик смог улететь ниже пола, проверяем его max_y_pos ниже видимой границы
			ball->Set_State(EBall_State::Lost, next_x_pos);
	}
	
	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
	int i;
	for(i = 0; i < AsConfig::Gates_Count; ++i)
		Gate[i]->Act();
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
	for (i = 0; i < AsConfig::Gates_Count; i++)
		Gate[i]->Clear(hdc, paint_area);

	if(AsConfig::Level_Has_Floor)
		AsCommon::Rect(hdc, Floor_Rect, AsConfig::BG_Color);
}
// нету реализации для методов AMover т.к. рамка не двигается

void AsBorder::Advance(double max_speed)
{
}
double AsBorder::Get_Speed()
{
	return 0.0;
}
void AsBorder::Start_Movement()
{
}
void AsBorder::End_Movement()
{
}
int AsBorder::Open_Long_Gate()
{
	int current_gate_index;
	AGate *gate = 0;
	while(true)
	{

		current_gate_index = AsCommon::Rand(AsConfig::Gates_Count - 1); // чтобы всегда выбирался только большой (не последний) гейт отнимаем единицу
		gate = Gate[current_gate_index];

		if(gate->Is_Closed() )
		{
			if(Gate[current_gate_index]->Level_X == -1 )
				break;

			if(!ALevel::Has_Brick_At(gate->Level_X, gate->Level_Y) && !ALevel::Has_Brick_At(gate->Level_X, gate->Level_Y + 1))
				break;
		}

		
	}

   Open_Gate(current_gate_index, false);

   return current_gate_index;
}

void AsBorder::Get_Gate_Pos(int index, double &x_pos, double &y_pos)
{
	if(index < 0 || index > AsConfig::Gates_Count)
		AsConfig::Throw();

	Gate[index]->Get_Pos(x_pos, y_pos);
}
void AsBorder::Open_Gate(int gate_index, bool is_open_short)
{
	if(gate_index >= 0 && gate_index < AsConfig::Gates_Count)
		Gate[gate_index]->Open_Gate(is_open_short);
	else
		AsConfig::Throw();

	if(gate_index != AsConfig::Gates_Count - 1 && is_open_short)
		AsConfig::Throw();
}
void AsBorder::Redraw_Floor()
{
	AsCommon::Invalidate_Rect(Floor_Rect);
}
bool AsBorder::Is_Gate_Opened(int gate_index)
{
	if(gate_index >= 0 && gate_index < AsConfig::Gates_Count)
		return Gate[gate_index]->Is_Opened();
	else
		AsConfig::Throw();

	return false;
}
bool AsBorder::Is_Gate_Closed(int gate_index)
{
	if(gate_index >= 0 && gate_index < AsConfig::Gates_Count)
		return Gate[gate_index]->Is_Closed();
	else
		AsConfig::Throw();

	return false;
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
		AsCommon::Rect(hdc, x, y + 1, 4, 3, AsConfig::Blue_Color);
	else
		AsCommon::Rect(hdc, x + 1, y, 3, 4, AsConfig::Blue_Color);

	// Белая кайма
	
	if (top_border)
		AsCommon::Rect(hdc, x, y, 4, 1, AsConfig::White_Color);
	else
		AsCommon::Rect(hdc, x, y, 1, 4, AsConfig::White_Color);

	// Перфорация
	if (top_border)
		AsCommon::Rect(hdc, x + 1, y + 2, 1, 1, AsConfig::BG_Color);
	else
		AsCommon::Rect(hdc, x + 2, y + 1, 1, 1, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT &paint_area)
{
	int i;
	int len_stroke, len_gap;
	int stroke_count;
	int stroke_len;
	double x_pos, y_pos;
	const int scale = AsConfig::Global_Scale;
	RECT intersection_rect;

	if(! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	len_stroke = 4 * scale;
	len_gap = 2 * scale;
	stroke_len = len_stroke + len_gap;
	stroke_count = (Floor_Rect.right - Floor_Rect.left) / stroke_len;

	x_pos = Floor_Rect.left;
	y_pos = Floor_Rect.top;
	AsConfig::Letter_Color.Select(hdc);
	for(i = 0; i < stroke_count; ++i)
	{
		MoveToEx(hdc, (int)x_pos, (int)y_pos, 0);
		LineTo(hdc, (int)x_pos + len_stroke, (int)y_pos);

		x_pos +=  (double)stroke_len;
	}

}