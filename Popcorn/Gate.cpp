#include "Gate.h"

const double AGate::Max_Gap_Short_Height = 9.0;
const double AGate::Gap_Short_Gate_Expanding_Step = Max_Gap_Short_Height / (double)AsConfig::FPS; // анимация проигрывается 1 секунду

const double AGate::Max_Gap_Long_Height = 18.0;
const double AGate::Gap_Long_Gate_Expanding_Step = Max_Gap_Long_Height / (double)AsConfig::FPS * 1.5;

AGate::AGate(int x_pos, double y_pos, int level_x, int level_y)
	: Level_X(level_x), Level_Y(level_y),  X_Pos(x_pos), Y_Pos(y_pos), Original_Y_Pos(y_pos),
	  Gap_Height(0.0), Gate_Close_Tick(0), Gate_State(EGate_State::Closed), Gate_Transformation(EGate_Transformation::Unknown)
{
	const int scale = AsConfig::Global_Scale;

	Gate_Rect.left = X_Pos * scale;
	Gate_Rect.top = (int)round(Original_Y_Pos * AsConfig::D_Global_Scale);
	Gate_Rect.right = Gate_Rect.left + AsConfig::Gate_Width * scale;
	Gate_Rect.bottom = Gate_Rect.top + AsConfig::Gate_Height * scale;

}
//------------------------------------------------------------------------------------------------------------
void AGate::Act()
{
	bool correct_pos;
	switch(Gate_State)
	{
		case EGate_State::Closed:
		  break;

		case EGate_State::Open_Short:
		case EGate_State::Open_Long:
			if(Act_For_Open(correct_pos) )
			{
				if(correct_pos)
					Correct_Gate_Y_Pos();

				Redraw_Gate();
			}
		  break;
	}
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
	bool is_short = true;
	if(! IntersectRect(&intersection_rect, &paint_area, &Gate_Rect) )
		return;

	
	Clear(hdc,paint_area); // для полной очистки места, где будет отрисовываться анимация (в том числе очистка границы)

	Draw_Cup(hdc, paint_area, true); // отрисовка верхней чаши гейта
	Draw_Cup(hdc, paint_area, false);// отрисовка нижней чаши гейта

	if(Gate_State == EGate_State::Open_Long && (Gate_Transformation == EGate_Transformation::Init 
		|| Gate_Transformation == EGate_Transformation::Finalize) )
	{
		Draw_Charge(hdc);
	}

}
//------------------------------------------------------------------------------------------------------------
void AGate::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect = {};

	if(! IntersectRect(&intersection_rect, &paint_area, &Gate_Rect) )
		return;

	AsCommon::Rect(hdc, Gate_Rect, AsConfig::BG_Color);
	
}
//------------------------------------------------------------------------------------------------------------
void AGate::Open_Gate(bool is_open_short)
{
	if(Gate_State != EGate_State::Closed)
		return;

	if(is_open_short)
		Gate_State = EGate_State::Open_Short;
	else
		Gate_State = EGate_State::Open_Long;

	Gate_Transformation = EGate_Transformation::Init;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Redraw_Gate()
{
	//нужно для того, чтобы заказать бОльшую область перерисовки
	Gate_Rect.top -= 4;
	Gate_Rect.bottom += 4;
	
	AsCommon::Invalidate_Rect(Gate_Rect);
	
	Gate_Rect.top += 4;
	Gate_Rect.bottom -= 4;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Opened()
{
	if(Gate_State != EGate_State::Closed && Gate_Transformation == EGate_Transformation::Active)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Closed()
{
	if(Gate_State == EGate_State::Closed)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Get_Pos(double &x_pos, double &y_pos)
{
	x_pos = (double)X_Pos;
	y_pos = Original_Y_Pos;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Cup(HDC hdc, RECT &paint_area, bool is_top_cup) 
{
	RECT rect = {}, intersection_rect = {};
	HRGN region;
	XFORM xform = {}, old_xform = {};
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	const int half_scale = AsConfig::Global_Scale / 2;
	int x = 0, y = 0;
	double cup_offset;
	
	xform.eM11 = 1.0f;
	xform.eM12 = 0.0f;
	xform.eM21 = 0.0f;
	xform.eDx = (float)X_Pos * scale;
	
	if(is_top_cup)
	{
		xform.eM22 = 1.0f;
		cup_offset = 0.0;
	}
	else
	{
		xform.eM22 = -1.0f;

		if(Gate_State == EGate_State::Open_Long)
			cup_offset = ( (double)AsConfig::Gate_Height+ Gap_Height) * d_scale - 1.0;
		else
			cup_offset = (double)AsConfig::Gate_Height * d_scale - 1.0;
	}

	xform.eDy = (float)round(Y_Pos * d_scale + cup_offset);

	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	// 1. Рисование синего основного прямоугольника
	
	rect.left = x * scale;
	rect.top = (y + 1) * scale;
	rect.right = rect.left + 6 * scale;
	rect.bottom = rect.top + 4 * scale;

	AsConfig::Blue_Color.Select(hdc);
	AsCommon::Round_Rect(hdc, rect, scale);

	// 1.1 Рисование белой каймы слева
	
	AsConfig::Letter_Color.Select_Pen(hdc);

	rect.left = X_Pos * scale;
	
	rect.right = rect.left + 3 * scale;
	rect.bottom = rect.top + 4 * scale;

	if(is_top_cup)
	{
		rect.top = (int)round( (Y_Pos + 1.0) * d_scale + cup_offset);
		rect.bottom = rect.top + 4 * scale;
	}
	else
	{
		rect.top = (int)round((Y_Pos - 1.0) * d_scale + cup_offset + 1.0);
		rect.bottom = rect.top - 4 * scale;
	}
   region = CreateRectRgnIndirect(&rect);
   SelectClipRgn(hdc, region);

	rect.left = x * scale				  + half_scale;
	rect.top = (y + 1) * scale			  + half_scale;
	rect.right = rect.left + 5 * scale + half_scale;
	rect.bottom = rect.top + 5 * scale + half_scale;

	AsCommon::Round_Rect(hdc, rect, scale);

   SelectClipRgn(hdc, 0);
   DeleteObject(region);

	// 1.2 Рисование нижней белой линии 
	MoveToEx(hdc, x * scale + 1, (y + 4) * scale + 1, 0);
	LineTo(hdc, (x + 4) * scale, (y + 4) * scale + 1);

	// 1.3 Рисование "заплатки"
	AsCommon::Rect(hdc, x + 4, y + 3, 2, 2, AsConfig::Blue_Color);

	// 1.4 Рисование перфорации
	AsCommon::Rect(hdc, x + 4, y + 3, 1, 1, AsConfig::BG_Color);

	// 1.5 Рисование перемычки над/снизу чаши
	AsCommon::Rect(hdc, x + 2, y + 0, 2, 1, AsConfig::Blue_Color);

	// 1.6 Рисование перемычек между двумя чашами
	Draw_Edges(hdc, paint_area);

	SetWorldTransform(hdc, &old_xform);

	
	
}
void AGate::Draw_Charge(HDC hdc)
{
	int i;
	double ratio = Gap_Height / AGate::Max_Gap_Short_Height;
	int dot_x;
	double dot_y;
	double field_y;
	field_y = Original_Y_Pos + (double)AsConfig::Gate_Height / 2.0 - Gap_Height / 2.0 + 1.0;

	if(ratio > 0.2)
	{
		for(i = 0; i < 4; ++i)
		{
			dot_x = 1 + AsCommon::Rand(4);
			
			dot_y = AsCommon::Rand((int)Gap_Height - 1);

			AsCommon::Rect(hdc, X_Pos + dot_x, (int) (field_y + dot_y), 1, 1, AsConfig::White_Color);
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Edges(HDC hdc, RECT &paint_area) 
{
	if(Gate_State == EGate_State::Open_Long && (Gate_Transformation != EGate_Transformation::Unknown) )
		Draw_Long_Edges(hdc);
	else
		Draw_Short_Edges(hdc);
	   
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Short_Edges(HDC hdc)
{
	
	int i;
	double ratio = 1.0 - Gap_Height / AGate::Max_Gap_Short_Height;
	
	bool is_blue = true;
	int count = (int)((double)AGate::Gates_Jumper_Count * ratio);

	for(i = 0; i < count; ++i) // отрисовка перемычек между двумя чашами
	{
		Draw_One_Edge(hdc, 5 + i, is_blue);

		is_blue = !is_blue;
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Long_Edges(HDC hdc)
{
	int i;
	bool is_blue = true;
	double ratio =  Gap_Height / AGate::Max_Gap_Short_Height;

	if(ratio < 0.3)
	{
		for(i = 0; i < 4;++i)
		{
			Draw_One_Edge(hdc, 5 + i, is_blue);

			is_blue = !is_blue;
		}
		if(ratio > 0.1)
			Draw_Red_Edge(hdc, 9, false, false);
	}
	else if(ratio < 0.5)
	{
		Draw_One_Edge(hdc, 5, true);
		Draw_One_Edge(hdc, 6, false);
		Draw_One_Edge(hdc, 7, false);

		Draw_Red_Edge(hdc, 8, true, true);
		Draw_Red_Edge(hdc, 9, false, false);
	}
	else
	{
		Draw_One_Edge(hdc, 5, false);
		Draw_One_Edge(hdc, 6, false);

		Draw_Red_Edge(hdc, 7, false, true);
		Draw_Red_Edge(hdc, 8, true, true);
		Draw_Red_Edge(hdc, 9, false, false);

	}
}

//------------------------------------------------------------------------------------------------------------
void AGate::Draw_One_Edge(HDC hdc, int edge_offset, bool is_blue) 
{ //!!! Отрефакторить
	int i;
	const int scale = AsConfig::Global_Scale;

	static int x_positions[] = {0, 1, 3, 4, 5};
	static int width[] = {1, 2, 1, 1, 1};
	static AColor colors[] = {AsConfig::BG_Color, AsConfig::Blue_Color, AsConfig::BG_Color, AsConfig::Blue_Color, AsConfig::BG_Color};
	if(is_blue)
	{
		for(i = 0; i < 5; ++i)
			AsCommon::Rect(hdc,0 + x_positions[i], 0 + edge_offset,width[i], 1, colors[i]);
	}									  
	else
	{
		AsCommon::Rect(hdc,0, 0 + edge_offset, 4, 1, AsConfig::White_Color);
		AsCommon::Rect(hdc,0 + 4, 0 + edge_offset, 2, 1, AsConfig::Blue_Color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Red_Edge(HDC hdc, int edge_offset, bool has_blick, bool is_long)
{
	const int scale = AsConfig::Global_Scale;

	if(is_long)
	{
		AsCommon::Rect(hdc, 0, 0 + edge_offset, 6, 1, AsConfig::Red_Color);

		if(has_blick)
			AsCommon::Rect(hdc, 0 + 1, 0 + edge_offset, 1, 1, AsConfig::White_Color);
	}
	else
		AsCommon::Rect(hdc, 0 + 1, 0 + edge_offset, 4, 1, AsConfig::Red_Color);
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Act_For_Open(bool &correct_pos)
{
	correct_pos = false;
	double max_gap_height, gap_gate_expanding_step;
	int opening_timeout;

	if(Gate_State == EGate_State::Open_Short)
	{
		max_gap_height = Max_Gap_Short_Height;
		gap_gate_expanding_step = Gap_Short_Gate_Expanding_Step;
		opening_timeout = Short_Opening_Timeout;
	}
	else
	{
		max_gap_height = Max_Gap_Long_Height;
		gap_gate_expanding_step = Gap_Long_Gate_Expanding_Step;
		opening_timeout = Long_Opening_Timeout;
	}
	switch(Gate_Transformation)
   {
   case EGate_Transformation::Init:
      if(Gap_Height < max_gap_height)
      {
         Gap_Height += gap_gate_expanding_step;
			if(Gap_Height > max_gap_height)
				Gap_Height = max_gap_height;

			if(Gate_State == EGate_State::Open_Long)
			   correct_pos = true;
      }
      
      else
		{
        Gate_Transformation = EGate_Transformation::Active;
		  Gate_Close_Tick = AsConfig::Current_Timer_Tick + opening_timeout;
		}

      return true;

   case EGate_Transformation::Active:

		if(Gate_Close_Tick <= AsConfig::Current_Timer_Tick )
			Gate_Transformation = EGate_Transformation::Finalize;

		break;
		 
   case EGate_Transformation::Finalize:
      if(Gap_Height > 0.0)
      {
	      Gap_Height -= gap_gate_expanding_step;
			if(Gap_Height < 0.0)
				Gap_Height = 0.0;

			if(Gate_State == EGate_State::Open_Long)
			   correct_pos = true;
  
      }
      else
		{
         Gate_Transformation = EGate_Transformation::Unknown;
			Gate_State = EGate_State::Closed;
		}
      

      return true;
   }

   return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Correct_Gate_Y_Pos()
{
	const double d_scale = AsConfig::D_Global_Scale;

	if(Gate_Transformation == EGate_Transformation::Init || Gate_Transformation == EGate_Transformation::Finalize)
	{
		Y_Pos = Original_Y_Pos - Gap_Height / 2.0;
		Gate_Rect.top = (int)round(Y_Pos * d_scale);
		Gate_Rect.bottom = (int)round( (Y_Pos + (double)AsConfig::Gate_Height + Gap_Height) * d_scale); 
	}
}