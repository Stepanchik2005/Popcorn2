#include "Indicator.h"


//AIndicator
AIndicator::AIndicator(int x_pos, int y_pos, AMessage message_type)
	: X_Pos(x_pos), Y_Pos(y_pos), End_Tick(0),Current_Height(0),Start_Animation(0),Indicator_Rect{}, Need_To_Notify(false), Message_Type(message_type)
{
	const int scale = AsConfig::Global_Scale;

	Indicator_Rect.left = X_Pos  * scale;
	Indicator_Rect.top = Y_Pos  * scale;
	Indicator_Rect.right = Indicator_Rect.left + Indicator_Width * scale;
	Indicator_Rect.bottom = Indicator_Rect.top + Indicator_Height * scale;
}

void AIndicator::Act()
{
	if(!Is_Finished())
		AsCommon::Invalidate_Rect(Indicator_Rect);
}
bool AIndicator::Is_Finished()
{
	if(AsConfig::Current_Timer_Tick > End_Tick && End_Tick > 0)
	{
		Cancel();
		return true;
	}
	else
		return false;
}

void AIndicator::Cancel()
{
	AMessage *message;

	if(Need_To_Notify)
	{
		message = new AMessage(Message_Type);
		AsMessage_Manager::Add_Message(message);
		Need_To_Notify = false;

		Reset();
	}
	
}
	
void AIndicator::Reset()
{
	End_Tick = 0;
	AsCommon::Invalidate_Rect(Indicator_Rect);
}
void AIndicator::Draw(HDC hdc, RECT &paint_area)
{
	RECT rect;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	double ratio;
	double curr_height;

	AsCommon::Rect(hdc, X_Pos, Y_Pos, Indicator_Width, Indicator_Height, AsConfig::Teleport_Color);

	if(Is_Finished() || End_Tick == 0)
		return;

	ratio = (double)(End_Tick - AsConfig::Current_Timer_Tick) / (double)Indicator_Timeout;

 	curr_height = (double)(Indicator_Inner_Height * scale) * ratio;

	rect.left = (X_Pos + 1) * scale;
	rect.top = (Y_Pos + 1) * scale + (Indicator_Inner_Height * scale - curr_height) ;
	rect.right = rect.left + Indicator_Inner_Width * scale;
	rect.bottom = (Y_Pos + 1 + Indicator_Inner_Height) * scale;
	
	if(rect.top == rect.bottom)
		return; // чтобы винддовс сама не рисовала однопиксельные ректангл

	AsCommon::Rect(hdc, rect, AsConfig::Red_Color);
}
	
void AIndicator::Clear(HDC hdc, RECT &paint_area)
{
	// Заглушка т.к. индикатор поностью перерисовывает себя
}

void AIndicator::Restart()
{
	End_Tick = AsConfig::Current_Timer_Tick + Indicator_Timeout;
	Need_To_Notify = true;
}



