#pragma once
#include "Config.h"
#include "Falling_Letter.h"
#include <string>

class AString
{
public:
	AString();
	AString(const wchar_t *str);

	const wchar_t * Get_Content();
	void Append(int val);

private:
	std::wstring Content;

};
class AsInfo_Panel: public AGame_Object
{
public:
	~AsInfo_Panel();
	AsInfo_Panel();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Init();
	
private:
	void Show_Extra_Lifes(HDC hdc);
	void Choose_Font();
	void Draw_String(HDC hdc, RECT &rect, AString &str, bool is_player_name);
	void Draw_Extra_Life(HDC hdc, int x_offset, int y_offset);

	HFONT Logo_Pop_Font, Logo_Corn_Font, Name_Font, Score_Font;

	AColor *Shadow_Color, *Highlight_Color, *Dark_Blue, *Dark_Red, *Platform_Inner_Color;

	AFalling_Letter Letter_P, Letter_G, Letter_M;

	static const int Score_X = 208;
	static const int Score_Y = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Offset = 22;
};