#include "Label.h"
// ALabel---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ALabel::ALabel()
{
}
ALabel::ALabel(int x_pos, int y_pos, int width, int height, const AFont font, const AColor &color)
	: X_Pos(x_pos), Y_Pos(y_pos), Width(width), Height(height), Font(font), Color(color)
{
	const int scale = AsConfig::Global_Scale;

	Content_Rect.left = X_Pos * scale;
	Content_Rect.top = Y_Pos * scale;
	Content_Rect.right = Content_Rect.left + Width * scale;
	Content_Rect.bottom = Content_Rect.top + Height * scale;
}

void ALabel::Draw(HDC hdc)
{
	// 1. Плажка с именем игрока
	const int scale = AsConfig::Global_Scale;
	int str_left_offset, str_top_offset;
	SIZE str_size;

	Font.Select(hdc);

	GetTextExtentPoint32(hdc, Content.Get_Content(), wcslen(Content.Get_Content()), &str_size);

	if(str_size.cx > Width * scale)
	{
		if(Last_Char != 0)
		{
			Content.Delete_Symbol();
			Last_Char = 0;
			GetTextExtentPoint32(hdc, Content.Get_Content(), wcslen(Content.Get_Content()), &str_size);
		}
	}

	str_left_offset = Content_Rect.left + (Content_Rect.right - Content_Rect.left) / 2 - str_size.cx / 2;
	str_top_offset = Content_Rect.top + (Content_Rect.bottom - Content_Rect.top) / 2  - str_size.cy / 2 - scale;

	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());

	TextOutW(hdc, str_left_offset + 2 * scale, str_top_offset + 2 * scale, Content.Get_Content(), wcslen(Content.Get_Content()));

	SetTextColor(hdc, Color.Get_RGB()); 
	TextOutW(hdc, str_left_offset, str_top_offset, Content.Get_Content(), wcslen(Content.Get_Content()));
}
void ALabel::Clear()
{
	Content = L""; // очищаем метку
}
bool ALabel::Append(wchar_t symbol)
{
	// 1. Поднимаем регистр английких букв
	if(symbol >= L'a' && symbol <= L'z')
		symbol -= L'a' - L'A';

	// 2. Поднимаем регистр кириллических букв
	if(symbol >= 0x430 && symbol <= 0x44f)
		symbol -= 0x20;

	if(symbol >= 0x450 && symbol <= 0x45f)
		symbol -= 0x50;

	if(symbol == 8) // 3. Удаляем символы особо
		Content.Delete_Symbol();
	else
	{ 
		if(symbol >= L' ')
		{
			Last_Char = symbol;
			Content.Append(symbol);
		}
	}
	AsCommon::Invalidate_Rect(Content_Rect);


	if(symbol ==  0x0d && Content.Get_Length() > 0)
		return true;
	else
		return false;
}

	


