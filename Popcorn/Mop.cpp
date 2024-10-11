#include "Mop.h"

//AsMop-------------------------------------------------------------------------------------------------------
AsMop::~AsMop()
{
	for(auto indicator : Mop_Indicators)
		delete indicator;

	Mop_Indicators.erase(Mop_Indicators.begin(), Mop_Indicators.end());

	for(auto cylinder : Mop_Cylinders)
		delete cylinder;

	Mop_Cylinders.erase(Mop_Cylinders.begin(), Mop_Cylinders.end());
}
AsMop::AsMop()
	:Y_Pos(0), Start_Tick(0), End_Tick(0), Max_Y_Pos(0), Lifting_Height(0)
{
	int i;
	int x_pos, y_pos;
	AMop_Indicator *indicator;
	AMop_Cylinder *cylinder;

	for(i = 0; i < Max_Indicators_Count; ++i)
	{
		indicator = new AMop_Indicator(AsConfig::Level_X_Offset + 2 + i * 19, AsConfig::Level_Y_Offset + 1, i * 80);
		Mop_Indicators.push_back(indicator);
	}

	for(i = 0; i < 4; ++i)
	{
		x_pos = AsConfig::Level_X_Offset + Width / 2  - 6 - i;
		y_pos = AsConfig::Level_Y_Offset + 7 + i * 5;

		cylinder = new AMop_Cylinder(x_pos, y_pos, 13 + i * 2, AMop_Cylinder::Max_Cylinder_Height[i]);
		Mop_Cylinders.push_back(cylinder);
	}
	
}
void AsMop::Advance(double max_speed)
{
}
double AsMop::Get_Speed()
{
	return 0.0;
}
void AsMop::Start_Movement()
{
}
void AsMop::End_Movement()
{
}

void AsMop::Act()
{

	if(Mop_State == EMop_State::Idle)
		return;

	Prev_Mop_Rect = Mop_Rect;

	switch(Mop_State)
	{
		case EMop_State::Ascending:
			if(! Act_For_Executing_State(Lifting_Timeout))
			{
				Mop_State = EMop_State::Cleaning;
				Start_Tick = AsConfig::Current_Timer_Tick;
			}

			break;

		case EMop_State::Cleaning:
			if(! Act_For_Executing_State(Expansion_Timeout))
			{
				Mop_State = EMop_State::Cleaning_Done;
				End_Tick = AsConfig::Current_Timer_Tick + Cleaning_Done_Timeout;
			}
		
			break;

		case EMop_State::Cleaning_Done:
			if (Act_For_Done_State() )
				Mop_State = EMop_State::Showing;

			break;

		case EMop_State::Showing:
			if(! Act_For_Executing_State(Expansion_Timeout))
			{
				Mop_State = EMop_State::Showing_Done;
				End_Tick = AsConfig::Current_Timer_Tick + Descending_Timeout;
			}

			break;

		case EMop_State::Showing_Done:
			if (Act_For_Done_State() )
				Mop_State = EMop_State::Descending;
				
			break;

		case EMop_State::Descending:
			if(! Act_For_Executing_State(Descending_Timeout) )
				Mop_State = EMop_State::Descending_Done;

			break;

		case EMop_State::Descending_Done:
			Mop_State = EMop_State::Idle;

			break;
			
	/*	default:
				AsConfig::Throw();*/
	}

	for(auto indicator : Mop_Indicators)
		indicator->Act();

	AsCommon::Invalidate_Rect(Prev_Mop_Rect);
	AsCommon::Invalidate_Rect(Mop_Rect);
}
bool AsMop::Is_Finished()
{
	return true;
}
void AsMop::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int x_pos = AsConfig::Level_X_Offset;
	RECT intersection_rect;

	if(Mop_State == EMop_State::Idle)
		return;

	if(!IntersectRect(&intersection_rect, &paint_area, &Mop_Rect) )
		return;

	Clean_Area(hdc);
	
	AsCommon::Rect(hdc, x_pos, Y_Pos, Width, Height, AsConfig::Red_Color);

	for(auto indicator : Mop_Indicators)
		indicator->Draw(hdc, paint_area);

	for(auto cylinder : Mop_Cylinders)
		cylinder->Draw(hdc, paint_area);

	
}
void AsMop::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Mop_State == EMop_State::Idle)
		return;

	if(!IntersectRect(&intersection_rect, &paint_area, &Prev_Mop_Rect) )
		return;
	
	AsCommon::Rect(hdc, Prev_Mop_Rect, AsConfig::BG_Color);
	
	for(auto cylinder : Mop_Cylinders)
		cylinder->Clear(hdc, paint_area);
}
void AsMop::Erase_Level()
{
	Start_Tick = AsConfig::Current_Timer_Tick;
	Y_Pos = 172;
	Mop_State = EMop_State::Ascending;
	Lifting_Height = Height + Get_Total_Cylinder_Height();

	Max_Y_Pos = AsConfig::Max_Y_Pos + Lifting_Height;

	Set_Mop();
}
void AsMop::Set_Mop()
{
	int i;
	const int scale = AsConfig::Global_Scale;
	int total_cylinder_height = 0;
	int curr_y_pos = 0;

	if(Mop_State == EMop_State::Idle)
		return;

	Y_Pos = Max_Y_Pos - Get_Total_Cylinder_Height() - Height + 1; 

	for(auto *indicator : Mop_Indicators)
		indicator->Set_Y_Pos(Y_Pos + 1);

	for(i = 0; i < Mop_Cylinders.size(); ++i)
	{
		Mop_Cylinders[i]->Set_Y_Pos(Y_Pos + Height + curr_y_pos);
			
		curr_y_pos += Mop_Cylinders[i]->Get_Height();
	}

	Mop_Rect.left = AsConfig::Level_X_Offset * scale;
	Mop_Rect.top = Y_Pos * scale;
	Mop_Rect.right = Mop_Rect.left + Width * scale;
	Mop_Rect.bottom = Mop_Rect.top + Height * scale;
}
bool AsMop::Is_Level_Cleaning_Done()
{
	if(Mop_State == EMop_State::Cleaning_Done)
		return true;
	else
		return false;
}
EMop_State AsMop::Get_State()
{
	return Mop_State;
}
void AsMop::Init()
{
	AMop_Indicator::Init();
}
void AsMop::Clean_Area(HDC hdc)
{
	RECT rect;

	rect = Mop_Rect;
	rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;

	AsCommon::Rect(hdc, rect, AsConfig::BG_Color);
}
int AsMop::Get_Total_Cylinder_Height()
{
	int total_cylinder_height = 0;

	for(auto *cylinder : Mop_Cylinders)
			total_cylinder_height += cylinder->Get_Height();

	return total_cylinder_height;
}

bool AsMop::Act_For_Executing_State(int timeout)
{
	double ratio;
	int time_offset = AsConfig::Current_Timer_Tick - Start_Tick;
	
	if(time_offset <= timeout)
	{
		ratio = (double)time_offset / (double)timeout;

		if(Mop_State == EMop_State::Showing || Mop_State == EMop_State::Ascending)// для обратной анимации (делаем коэфф от 1 до 0
			ratio = 1.0 - ratio;

		switch(Mop_State)
		{
		case EMop_State::Ascending:
		case EMop_State::Descending:
			Max_Y_Pos = AsConfig::Max_Y_Pos + (int)( (double)Lifting_Height * ratio);

			break;

		case EMop_State::Cleaning:
		case EMop_State::Showing:
			
			for(auto cylinder : Mop_Cylinders)
				cylinder->Set_Height_For(ratio);

			break;
		}

		Set_Mop();
		return true;
	}
	else
		return false;
}
bool AsMop::Act_For_Done_State()
{
	if(AsConfig::Current_Timer_Tick > End_Tick)
	{
		Start_Tick = AsConfig::Current_Timer_Tick;
		return true;
	}
	else
		return false;
}