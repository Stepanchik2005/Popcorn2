#pragma once
#include "Config.h"
#include "Falling_Letter.h"
#include "Indicator.h"
#include <string>

enum class EHit_Type
{
	Hit_Brick,
	Hit_Monster,
	Catch_Letter
};




class AString
{
public:
	AString();
	AString(const wchar_t *str);

	const wchar_t *Get_Content();
	void Append(int val, int digits);

private:
	std::wstring Content;

};


class ALabel
{
public:
	ALabel();
	ALabel(int x_pos, int y_pos, int width, int height,const AFont font,const AColor &color);

	void Draw(HDC hdc);
	void Clear();

	AFont Font;
	AColor Color;
	AString Content;
	RECT Content_Rect;

private:
	int X_Pos, Y_Pos;
	int Width, Height;
};



//-------------------------------------------------------------------------------------
class	AsInfo_Panel: public AGame_Object
{
public:

	AsInfo_Panel();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Increase_Life_Count();
	bool Decrease_Life_Count();

	static void Update_Score(EHit_Type hit_type);

	AIndicator Floor_Indicator, Monster_Indicator;
private:
	void Show_Extra_Lifes(HDC hdc);
	void Draw_Extra_Life(HDC hdc, int x_offset, int y_offset);

   int Availiable_Extra_Lifes;

	AColor Dark_Blue, Dark_Red, Platform_Inner_Color;
	ALabel Player_Name_Label, Score_Label;

	AFalling_Letter Letter_P, Letter_G, Letter_M;
	static RECT Logo_Rect;
	static RECT Data_Rect;
	static int Score;

	static const int Score_X = 208;
	static const int Score_Y = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Y_Offset = 22;

	static const int Logo_X_Pos = 212;
	static const int Logo_Y_Pos =  0;
	static const int Logo_Width = 104;
	static const int Logo_Height = 100;
	static const int Shadow_X_Offset = 5;
	static const int Shadow_Y_Offset = 5;
	static const int Indicator_Y_Offset = 55;
};


