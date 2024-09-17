#include "Info_Panel.h"

//AString
AString::AString()
{
}
AString::AString(const wchar_t *str)
	: Content(str)
{
	
}
const wchar_t *AString::Get_Content()
{
	return Content.c_str();
}

void AString::Append(int val)
{
	wchar_t buf[32];

	swprintf_s(buf, L"%.6i", val);

	Content += buf;
}




// AsInfo_Panel
int AsInfo_Panel::Score = 0;
RECT AsInfo_Panel::Logo_Rect;
RECT AsInfo_Panel::Data_Rect;

AsInfo_Panel::~AsInfo_Panel()
{
	delete Dark_Blue;
	delete Platform_Inner_Color;
}
AsInfo_Panel::AsInfo_Panel()
	:Availiable_Extra_Lifes(AsConfig::Initiall_Extra_Lives),
	 Floor_Indicator(Score_X + 8, Score_Y + Indicator_Y_Offset, EMessage_Type::Floor_Is_Over), Monster_Indicator(Score_X + 90, Score_Y + Indicator_Y_Offset, EMessage_Type::Unfreeze_Monsters),
	 Logo_Pop_Font(0), Logo_Corn_Font(0), Score_Font(0), Player_Name(L"COMPUTER"),Dark_Blue(0), Dark_Red(0), Platform_Inner_Color(0),
	 Letter_P (ELetter_Type::P, 215 * AsConfig::Global_Scale - 1, 153 * AsConfig::Global_Scale, EBrick_Type::Blue),
	 Letter_G (ELetter_Type::G, 256 * AsConfig::Global_Scale - 1, 153 * AsConfig::Global_Scale, EBrick_Type::Blue),
	 Letter_M (ELetter_Type::M, 296 * AsConfig::Global_Scale + 1, 153 * AsConfig::Global_Scale , EBrick_Type::Blue)
{
	const int scale = AsConfig::Global_Scale;

	Choose_Font();

	Letter_P.Show_Fill_Size();
	Letter_G.Show_Fill_Size();
	Letter_M.Show_Fill_Size();

	Logo_Rect.left = (Logo_X_Pos - 3) * scale; // берем меньше т.к. буква C в слове"Corn" немного вылазит назад
	Logo_Rect.top = (Logo_Y_Pos + 5) * scale; // 5 - это смещение относительно самого верха экрана т.к. Logo_Y_Pos = 0
	Logo_Rect.right = Logo_Rect.left + Logo_Width * scale;
	Logo_Rect.bottom = Logo_Rect.top + Logo_Height * scale;

	Data_Rect.left = Score_X * scale;
	Data_Rect.top = Score_Y * scale;
	Data_Rect.right = Data_Rect.left + Score_Width * scale;
	Data_Rect.bottom = Data_Rect.top + Score_Height * scale;
}

void AsInfo_Panel::Advance(double max_speed)
{

}
double AsInfo_Panel::Get_Speed()
{
	return 0.0;
}
void AsInfo_Panel::Start_Movement()
{
}
void AsInfo_Panel::End_Movement()
{
}

void AsInfo_Panel::Act()
{
	Floor_Indicator.Act();
	Monster_Indicator.Act();
}
bool AsInfo_Panel::Is_Finished()
{
	return false;
}

void AsInfo_Panel::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	wchar_t buf[32]{};
   const wchar_t *pop_str = L"POP";
	const wchar_t *corn_str = L"CORN";
	int corn_offset = 48;
	AString score(L"SCORE:");
	RECT rect = {}, intersection_rect;

	// 1. Логотип
	if ( IntersectRect(&intersection_rect, &paint_area, &Logo_Rect) )
	{
		AsCommon::Rect(hdc, Logo_X_Pos - 1, Logo_Y_Pos + 5, Logo_Width, Logo_Height, AsConfig::Blue_Color);

		SetBkMode(hdc, TRANSPARENT);

		SelectObject(hdc, Logo_Pop_Font);

		SetTextColor(hdc, AsConfig::BG_Color.Get_RGB()); // POP
		TextOutW(hdc, (Logo_X_Pos + Shadow_X_Offset) * scale, (Logo_Y_Pos + Shadow_Y_Offset) * scale, pop_str, wcslen(pop_str)); 

		SetTextColor(hdc, AsConfig::Red_Color.Get_RGB()); // POP
		TextOutW(hdc, Logo_X_Pos * scale, Logo_Y_Pos * scale, pop_str, wcslen(pop_str));
	
		SelectObject(hdc, Logo_Corn_Font);

		SetTextColor(hdc, AsConfig::BG_Color.Get_RGB()); //CORN
		TextOutW(hdc, (Logo_X_Pos + Shadow_X_Offset - 5) * scale, (Logo_Y_Pos + Shadow_Y_Offset + corn_offset) * scale, corn_str, wcslen(corn_str));
		SetTextColor(hdc, AsConfig::Red_Color.Get_RGB()); //CORN
		TextOutW(hdc, (Logo_X_Pos - 5) * scale, (Logo_Y_Pos + corn_offset) * scale, corn_str, wcslen(corn_str));
	}

	// 2. Таблица счета
	if ( IntersectRect(&intersection_rect, &paint_area, &Data_Rect) )
	{
		AsCommon::Rect(hdc, Score_X, Score_Y, 2, Score_Height, *Dark_Red);
		AsCommon::Rect(hdc, Score_X, Score_Y, Score_Width, 2,  *Dark_Red);
		AsCommon::Rect(hdc, Score_X + Score_Width - 2, Score_Y, 2, Score_Height, *Dark_Red);
		AsCommon::Rect(hdc, Score_X + Score_Width - 1, Score_Y + Score_Height - 2, -Score_Width + 2, 2, *Dark_Red);

		AsCommon::Rect(hdc, Score_X + 2 , Score_Y + 2, Score_Width - 4, Score_Height - 4, *Dark_Blue);

		// 2.1 Линии
		AsConfig::Highlight_Color.Select(hdc);
		MoveToEx(hdc, (Score_X + 2) * scale, (Score_Y + Score_Height - 2) * scale, 0); 
		LineTo(hdc, (Score_X + 2) * scale, (Score_Y + 2) * scale); // левая
		LineTo(hdc, (Score_X + Score_Width - 2) * scale, (Score_Y + 2) * scale); // верхняя

		AsConfig::Shadow_Color.Select(hdc);
		MoveToEx(hdc, (Score_X + Score_Width - 2) * scale, (Score_Y + 2) * scale, 0);
		LineTo(hdc, (Score_X + Score_Width - 2) * scale, (Score_Y + Score_Height - 2) * scale);
		LineTo(hdc, (Score_X + 2) * scale, (Score_Y + Score_Height - 2) * scale);

		// 2.2 Строки
		rect.left = (Score_X + 5) * scale;
		rect.top =  (Score_Y + 5) * scale;
		rect.right = rect.left + (Score_Width - 2 * 5) * scale;
		rect.bottom = rect.top + 16 * scale;

		Draw_String(hdc, rect, Player_Name, true);

		rect.top += Score_Offset * scale;
		rect.bottom += Score_Offset * scale;

		score.Append(Score); // сливаем две строки

		Draw_String(hdc, rect, score, false);

		// 3. Выводим буквы
		Letter_P.Draw(hdc, paint_area);
		Letter_G.Draw(hdc, paint_area);
		Letter_M.Draw(hdc, paint_area);


		// 4.Индикаторы
		Floor_Indicator.Draw(hdc, paint_area); // левый
		AsCommon::Rect(hdc, Score_X + 28, Score_Y + Indicator_Y_Offset, 54, 30, AsConfig::Teleport_Color); // средний
		Monster_Indicator.Draw(hdc, paint_area); // правый

		// 5. Рисуем жизни
		Show_Extra_Lifes(hdc);
	}

	


}

void AsInfo_Panel::Clear(HDC hdc, RECT &paint_area)
{
}
void AsInfo_Panel::Init()
{
	
	Dark_Blue = new AColor(0, 170, 170);
	Dark_Red = new AColor(151, 0, 0);
	Platform_Inner_Color = new AColor(0, 128, 192);
}
void AsInfo_Panel::Increase_Life_Count()
{
	if(Availiable_Extra_Lifes >= AsConfig::Max_Life_Count)
			return;

	Availiable_Extra_Lifes++;

	AsCommon::Invalidate_Rect(Data_Rect);

}
bool AsInfo_Panel::Decrease_Life_Count()
{
	if(Availiable_Extra_Lifes <= 0)
		return false;

  Availiable_Extra_Lifes--;

  AsCommon::Invalidate_Rect(Data_Rect);

  return true;
}
void AsInfo_Panel::Update_Score(EHit_Type hit_type)
{
	RECT rect = {};

	switch(hit_type)
	{
		case EHit_Type::Hit_Brick:
			Score += 20;
			break;

		case EHit_Type::Hit_Monster:
			Score += 37;
			break;

		case EHit_Type::Catch_Letter:
			Score += 23;
			break;
	}

	AsCommon::Invalidate_Rect(Data_Rect);

}
void AsInfo_Panel::Show_Extra_Lifes(HDC hdc)
{	// Рисуем жизни
	int i, j;
	const int scale = AsConfig::Global_Scale;
	int lives_to_draw = Availiable_Extra_Lifes;
	int x_platform_offset = (Score_X + 28) * scale;
	int y_platform_offset =  (Score_Y + 55) * scale - 1;
	for(i = 0; i < 3; ++i)
		for(j = 0; j < 4; j++)
		{
			if(lives_to_draw <= 0)
				break;

			Draw_Extra_Life(hdc, x_platform_offset + i * 50, y_platform_offset + j * 17); // 50 и 17 - это расстояние по осям между соответствующими координатами соседних платформ

			lives_to_draw--;
		}
}
void AsInfo_Panel::Choose_Font()
{
    LOGFONT logo_font{}; 
   
 
	 logo_font.lfHeight = -128;
	 logo_font.lfWeight = 900;
	 logo_font.lfOutPrecision = 3;
	 logo_font.lfClipPrecision = 2;
	 logo_font.lfQuality = 1;
	 logo_font.lfPitchAndFamily = 34;
	 wcscpy_s(logo_font.lfFaceName, L"Arial Black");

	 Logo_Pop_Font = CreateFontIndirect(&logo_font); 

	 logo_font.lfHeight = -96;
	 Logo_Corn_Font = CreateFontIndirect(&logo_font); 

	
		
	 logo_font.lfHeight = -48;
	 logo_font.lfWeight = 700;
	 logo_font.lfOutPrecision = 3;
	 logo_font.lfClipPrecision = 2;
	 logo_font.lfQuality = 1;
	 logo_font.lfPitchAndFamily = 49;
	 wcscpy_s(logo_font.lfFaceName, L"Consolas");

	 Name_Font = CreateFontIndirect(&logo_font); 

	 logo_font.lfHeight = -44;
	 Score_Font = CreateFontIndirect(&logo_font); 
}
void AsInfo_Panel::Draw_String(HDC hdc, RECT &rect, AString &str, bool is_player_name)
{
	// 3. Плажка с именем игрока
	const int scale = AsConfig::Global_Scale;
	int str_left_offset, str_top_offset;
	SIZE str_size;
	const AColor *color = 0;

	// 1. Выводим плашку фона
	AsCommon::Rect(hdc, rect, *Dark_Red);

	// 2. Выводим строку
	if(is_player_name)
	{
		SelectObject(hdc, Name_Font);
		color = &AsConfig::Blue_Color;
	}
	else
	{
		SelectObject(hdc, Score_Font);
		color = &AsConfig::White_Color;
	}
	
	GetTextExtentPoint32(hdc, str.Get_Content(), wcslen(str.Get_Content()), &str_size);

	str_left_offset = rect.left + (rect.right - rect.left) / 2 - str_size.cx / 2;
	str_top_offset = rect.top + (rect.bottom - rect.top) / 2  - str_size.cy / 2 - scale;

	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB());

	TextOutW(hdc, str_left_offset + 2 * scale, str_top_offset + 2 * scale, str.Get_Content(), wcslen(str.Get_Content()));

	SetTextColor(hdc, color->Get_RGB()); 
	TextOutW(hdc, str_left_offset, str_top_offset, str.Get_Content(), wcslen(str.Get_Content()));
}
void AsInfo_Panel::Draw_Extra_Life(HDC hdc, int x_offset, int y_offset)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect{};
	int ball_size = 4 * scale;
	int platform_width = 8 * scale + 1;

	rect.left = x_offset + 5 * scale;
	rect.top = y_offset + 5 * scale;
	rect.right = rect.left + ball_size;
	rect.bottom = rect.top + ball_size - 1;

	AsCommon::Ellipse(hdc, rect, *Dark_Red);

	rect.left += 7 * scale;
	rect.right = rect.left + ball_size;

	AsCommon::Ellipse(hdc, rect, *Dark_Red);

	rect.left = x_offset + 5 * scale + ball_size / 2 - 2;
	rect.top += 1;
	rect.right = rect.left + platform_width;
	rect.bottom -= 1;

	Platform_Inner_Color->Select(hdc);
	AsCommon::Round_Rect(hdc, rect);
}




