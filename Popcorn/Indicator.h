#pragma once
#include "Config.h"
//-------------------------------------------------------------------------------------
class AIndicator: public AGraphics_Objects
{
public:
	AIndicator(int x_pos, int y_pos, AMessage message_type);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Restart();
	void Cancel();
	void Reset();
private:
	

	int X_Pos, Y_Pos;
	int End_Tick;
	double Current_Height;
	int Start_Animation;
	RECT Indicator_Rect;
	bool Need_To_Notify;
	AMessage Message_Type;

	static const int Indicator_Width = 12;
	static const int Indicator_Height = 30;
	static const int Indicator_Inner_Width = Indicator_Width - 2;
	static const int Indicator_Inner_Height = Indicator_Height - 2;
	static const int Indicator_Timeout = AsConfig::FPS * 50;
};


