#include "Final_Letter.h"
//AFinal_Letter------------------------------------------------------------------------------------------------------
AFinal_Letter::AFinal_Letter(int x_pos, int y_pos, const wchar_t letter)
	:X_Pos(x_pos), Y_Pos(y_pos), Letter(letter), Final_Letter_State(EFinal_Letter_State::Idle), Start_Exploding_Tick(0), /*Width(0), Height(0),  */Color(AsConfig::White_Color.R, AsConfig::White_Color.G, AsConfig::White_Color.B), Got_Letter_Size(false)
{
}

void AFinal_Letter::Act()
{
	const int scale = AsConfig::Global_Scale;
	int count = 0;
	switch(Final_Letter_State)
	{
	case EFinal_Letter_State::Idle:
	case EFinal_Letter_State::Descending:
		break;

	case EFinal_Letter_State::Stopped:
		if(AsConfig::Current_Timer_Tick > Start_Exploding_Tick) // задаем каждому мячику большее значение на константу, а потом ждем, чтобы текущий тик таймера превысил это значение
		{
			Final_Letter_State = EFinal_Letter_State::Hiding;
			Redraw_Rect();
		}
		break;

	case EFinal_Letter_State::Hiding:
	
		// один раз активируем мячики
		Activate_Explosion(Final_Letter_Rect);
		Final_Letter_State = EFinal_Letter_State::Exploding;
		
		break;

	case EFinal_Letter_State::Exploding: // только для "КОНЕЦ ИГРЫ"
		Act_Explosion(count);

		if(count == Explodive_Balls_Count) // если все шарики отыграли свою анимацию, то состояния перехоит в первичное
			Final_Letter_State = EFinal_Letter_State::Finished;

		break;

	case EFinal_Letter_State::Finished:
			break;
	}
}
bool AFinal_Letter::Is_Finished()
{
	return false;
}
void AFinal_Letter::Draw(HDC hdc, RECT &paint_area)
{
  
	switch(Final_Letter_State)
	{
		case EFinal_Letter_State::Idle:
		case EFinal_Letter_State::Hiding: // в этом состоянии надо только стереть букву методом Clear()
		case EFinal_Letter_State::Finished:
			return;
			 
		case EFinal_Letter_State::Exploding:
			Draw_Explosion(hdc, paint_area);

			break;

		case EFinal_Letter_State::Descending:
		case EFinal_Letter_State::Stopped:
			if(! Got_Letter_Size)
			{
			/*	SIZE letter_size;

				GetTextExtentPoint32A(hdc, (LPCSTR)&Letter, 1, &letter_size);

				Width = letter_size.cx;
				Height = letter_size.cy; 

				Got_Letter_Size = true;*/
			}

			SetBkMode(hdc, TRANSPARENT);

			AsConfig::Final_Title_Font.Select(hdc);

			SetTextColor(hdc, Color.Get_RGB()); 
			TextOutW(hdc, X_Pos * AsConfig::Global_Scale, Y_Pos * AsConfig::Global_Scale, &Letter, 1); 

			break;



	}
}
void AFinal_Letter::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(! IntersectRect(&intersection_rect, &paint_area, &Final_Letter_Rect))
		return;

	if(Final_Letter_State != EFinal_Letter_State::Hiding) // очищаем буквы только для этого состояния 
		return;

	AsConfig::Final_Title_Font.Select(hdc);

	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB()); 

	TextOutW(hdc, X_Pos * AsConfig::Global_Scale, Y_Pos * AsConfig::Global_Scale, &Letter, 1); 
}
void AFinal_Letter::Redraw_Rect()
{
	const int scale = AsConfig::Global_Scale;

	Final_Letter_Rect.left = X_Pos * scale;
	Final_Letter_Rect.top = Y_Pos * scale;
	Final_Letter_Rect.right = Final_Letter_Rect.left + Width * scale;
	Final_Letter_Rect.bottom = Final_Letter_Rect.top + Height * scale;

	AsCommon::Invalidate_Rect(Final_Letter_Rect);

	
}
void AFinal_Letter::Change_Color(unsigned char r, unsigned char g,unsigned char b)
{
	Color.Set_As(r, g, b);

	Redraw_Rect();

}