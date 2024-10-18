#include "Level_Title.h"

//AsLevel_Table-----------------------------------------------------------------------------------------------------------
AsLevel_Title::AsLevel_Title(int x_pos, int y_pos)
	: X_Pos(x_pos), Y_Pos(y_pos), Level_Title_State(ELevel_Title_State::Missing), Level_Label(X_Pos + 10, Y_Pos + 3, AsConfig::Max_X_Pos / 2 - Width / 2, Height - 5, AsConfig::Name_Font, AsConfig::White_Color),
	  Level_Number_Label(X_Pos + Width - 25, Y_Pos + 3, 20, Height - 5, AsConfig::Name_Font, AsConfig::White_Color),
	  Level_Number(0)
{
	const int scale = AsConfig::Global_Scale;

	Level_Table_Rect.left = X_Pos * scale;
	Level_Table_Rect.top = Y_Pos * scale;
	Level_Table_Rect.right = Level_Table_Rect.left + Width * scale;
	Level_Table_Rect.bottom = Level_Table_Rect.top + Height * scale;
}

void AsLevel_Title::Act()
{
	// заглушка, т.к нету своей анимации
}
bool AsLevel_Title::Is_Finished()
{
	return false; // заглушка, т.к нету своей анимации
}
void AsLevel_Title::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Level_Title_State == ELevel_Title_State::Missing)
		return;

	if(IntersectRect(&intersection_rect, &paint_area, &Level_Table_Rect))
	{
		// 1. Рисуем плажку
		AsCommon::Rect(hdc, Level_Table_Rect, AsConfig::Blue_Color);

		Level_Label.Content = L"УРОВЕНЬ";

		Level_Label.Draw(hdc);

		Level_Number_Label.Content.Append(Level_Number, 2);

		Level_Number_Label.Draw(hdc);

	}
}
void AsLevel_Title::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Level_Title_State != ELevel_Title_State::Hidding)
		return;

	if(IntersectRect(&intersection_rect, &paint_area, &Level_Table_Rect))
	{
		AsCommon::Rect(hdc, Level_Table_Rect, AsConfig::BG_Color);
		Level_Title_State = ELevel_Title_State::Missing;
		Level_Number_Label.Clear();
	}
}

void AsLevel_Title::Show(int level_number)
{
	Level_Title_State = ELevel_Title_State::Showing;

	Level_Number = level_number;

	AsCommon::Invalidate_Rect(Level_Table_Rect);
}
void AsLevel_Title::Hide()
{
	Level_Title_State = ELevel_Title_State::Hidding;

	AsCommon::Invalidate_Rect(Level_Table_Rect);
}


