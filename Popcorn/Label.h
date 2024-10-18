#pragma once
#include "Config.h"
class ALabel
{
public:
	ALabel();
	ALabel(int x_pos, int y_pos, int width, int height,const AFont font,const AColor &color);

	void Draw(HDC hdc);
	void Clear();
	bool Append(wchar_t symbol);

	AFont Font;
	AColor Color;
	AString Content;
	RECT Content_Rect;
	
private:
	int X_Pos, Y_Pos;
	int Width, Height;
	wchar_t Last_Char;
};
