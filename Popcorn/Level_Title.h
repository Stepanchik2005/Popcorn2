#pragma once
#include "Config.h"
#include "Label.h"
//------------------------------------------------------------------------------------------------------------
enum class ELevel_Title_State
{
	Missing,
	Showing,
	Hidding
};
//------------------------------------------------------------------------------------------------------------
class AsLevel_Title: public AGraphics_Objects
{
public:
	AsLevel_Title(int x_pos, int y_pos);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Show(int level_number);
	void Hide();

private:
	int X_Pos, Y_Pos;
	int Level_Number;

	RECT Level_Table_Rect;
	ELevel_Title_State Level_Title_State;

	ALabel Level_Label, Level_Number_Label;

	static const int Width = 96;
	static const int Height = 14;
};