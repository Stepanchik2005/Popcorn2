#include "Final_Title.h"
//AsFinal_Title-------------------------------------------------------------------------------------------------
AsFinal_Title::~AsFinal_Title()
{
	
	for(auto letter : Final_Letters)
		delete letter;

	Final_Letters.erase(Final_Letters.begin(), Final_Letters.end());
}

AsFinal_Title::AsFinal_Title()
	: Start_Tick(0), Is_Won(0)
{
	
}

void AsFinal_Title::Act()
{
	int curr_tick;
	double ratio;
	int y_pos;

	if(Final_Title_State == EFinal_Title_State::Missing || Final_Title_State == EFinal_Title_State::Finished)
		return;

	curr_tick = AsConfig::Current_Timer_Tick - Start_Tick;


	switch(Final_Title_State)
	{
	case EFinal_Title_State::Missing:
		break;

	case EFinal_Title_State::Won_Descending:
	case EFinal_Title_State::Game_Over_Descending:

		if(curr_tick < Descend_Timeout)
			 ratio = (double)curr_tick / (double)Descend_Timeout;
	
		else
		{
			Final_Title_State = EFinal_Title_State::Game_Over_Stopped;
			ratio = 1.0;
			
		}
		y_pos = Low_Y_Pos * ratio;

		for(auto letter : Final_Letters)
			letter->Y_Pos = y_pos;
		

		Redraw_Title();

		break;

	case EFinal_Title_State::Game_Over_Stopped:

		curr_tick = AsConfig::Current_Timer_Tick;

		for(auto letter : Final_Letters)
		{
			if(Is_Won)
			{
				Final_Title_State = EFinal_Title_State::Game_Won_Animate;
				break;
			}
			else
			{
				letter->Final_Letter_State = EFinal_Letter_State::Stopped;
				letter->Start_Exploding_Tick = curr_tick;
				curr_tick += Start_Explosion_Offset;
				Final_Title_State = EFinal_Title_State::Game_Over_Exploding;
			}
			
		}

		break;

	case EFinal_Title_State::Game_Over_Exploding:
		for(auto letter : Final_Letters) // нужно анимировать только для этого состояния 
			letter->Act();

		break;

	case EFinal_Title_State::Game_Won_Animate:
		if(curr_tick < Game_Won_Animate_Timeout)
			Animate_Won_State();
		else
			Final_Title_State = EFinal_Title_State::Finished;
		break;

	case EFinal_Title_State::Finished:
		break;
	}
	
	
}
bool AsFinal_Title::Is_Finished()
{
	return false;
}
void AsFinal_Title::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(IntersectRect(&intersection_rect, &paint_area, &Prev_Final_Title_Rect))
	{
		for(auto letter : Final_Letters)
			letter->Draw(hdc, paint_area);
	}
}
void AsFinal_Title::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(IntersectRect(&intersection_rect, &paint_area, &Prev_Final_Title_Rect))
	{
		AsCommon::Rect(hdc, Prev_Final_Title_Rect, AsConfig::BG_Color);
	}

	for(auto letter : Final_Letters)
		letter->Clear(hdc, paint_area);
}
void AsFinal_Title::Show(bool is_win)
{
	int title_x, title_y = 0;
	const int scale = AsConfig::Global_Scale;

	// при желании можно будет переделать
	if(is_win)
	{
		title_x = 51; 
		// !!! можно сделать отрицательные значения для Y_Pos, чтобы создать эффект выхода из рамки
		Final_Letters.push_back(new AFinal_Letter(title_x, 0, L'П'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 16, 0, L'О'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 34, 0, L'Б'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 49, 0, L'Е'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 63, 0, L'Д'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 81, 0, L'А'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 98, 0, L'!'));

		Final_Title_State = EFinal_Title_State::Won_Descending;
	}
	else
	{
		title_x = 32;

		Final_Letters.push_back(new AFinal_Letter(title_x, title_y, L'К'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 14, title_y, L'О'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 31, title_y, L'Н'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 47, title_y, L'Е'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 61, title_y, L'Ц'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 83, title_y, L'И'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 99, title_y, L'Г'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 111, title_y, L'Р'));
		Final_Letters.push_back(new AFinal_Letter(title_x + 123, title_y, L'Ы'));

		Final_Title_State = EFinal_Title_State::Game_Over_Descending;
	}

	 
	 for(auto letter : Final_Letters)
		letter->Final_Letter_State = EFinal_Letter_State::Descending;

	Start_Tick = AsConfig::Current_Timer_Tick;

	Is_Won = is_win;

	Redraw_Title();
}
bool AsFinal_Title::Is_Visible()
{
	if(Final_Title_State == EFinal_Title_State::Missing || Final_Title_State == EFinal_Title_State::Finished)
		return false;
	else
		return true;	
}
void AsFinal_Title::Redraw_Title()
{
	const int scale = AsConfig::Global_Scale;

	Prev_Final_Title_Rect = Final_Title_Rect;

	Final_Title_Rect.left = Final_Letters[0]->X_Pos * scale;
	Final_Title_Rect.top = Final_Letters[0]->Y_Pos * scale;
	Final_Title_Rect.right =(Final_Letters[Final_Letters.size() - 1]->X_Pos + 24) * scale;
	Final_Title_Rect.bottom = Final_Title_Rect.top + Max_Letter_Height * scale;

	AsCommon::Invalidate_Rect(Prev_Final_Title_Rect);
	//AsCommon::Invalidate_Rect(Final_Title_Rect); // !!! Баг, иначе нельзя, не понимаю почему
}
void AsFinal_Title::Animate_Won_State()
{
	int letter_index;
	int r,g,b;

	letter_index = AsCommon::Rand(Final_Letters.size());

	r = AsCommon::Rand(256);
	g = AsCommon::Rand(256);
	b = AsCommon::Rand(256);
   
	Final_Letters[letter_index]->Change_Color(r, g, b);
}