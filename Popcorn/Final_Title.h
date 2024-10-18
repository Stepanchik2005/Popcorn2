#pragma once
#include "Config.h"
#include "Final_Letter.h"
enum class EFinal_Title_State
{
	Missing,

	Won_Descending,
	Game_Over_Descending,

	Game_Over_Stopped,
	Game_Over_Exploding,

	Game_Won_Animate,

	Finished
};


class AsFinal_Title: public AGraphics_Objects
{
public:
	~AsFinal_Title();
	AsFinal_Title();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Show(bool is_win);
	bool Is_Visible();

private:
	void Redraw_Title();
	void Animate_Won_State();


	int Start_Tick;
	bool Is_Won;
	EFinal_Title_State Final_Title_State;
	RECT Final_Title_Rect, Prev_Final_Title_Rect;



	std::vector<AFinal_Letter *> Final_Letters;
	
	static const int Descend_Timeout = AsConfig::FPS * 6;
	static const int Low_Y_Pos = 135;
	static const int Max_Letter_Height = 32;
	static const int Start_Explosion_Offset = 10; // каждый следующий взрыв будет начинаться позже на это значение
	static const int Game_Won_Animate_Timeout = AsConfig::FPS * 30;

};

