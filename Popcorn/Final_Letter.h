#pragma once
#include "Config.h"
#include "Explosion.h"
enum class EFinal_Letter_State
{
	Idle,

	Descending,
	Stopped,
	Hiding,
	Exploding,

	Finished
};
class AFinal_Letter: public AGraphics_Objects, public AsExplosion
{
public:
	AFinal_Letter(int x_pos, int y_pos, const wchar_t letter);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Change_Color(unsigned char r, unsigned char g,unsigned char b);

	double X_Pos, Y_Pos;

	EFinal_Letter_State Final_Letter_State;
	int Start_Exploding_Tick;
	/*int Width;
	int Height;*/

	static const int Width = 24;
	static const int Height = 32;
private:
	void Redraw_Rect();

	wchar_t Letter;
   bool Got_Letter_Size;

	AColor Color;
	RECT Final_Letter_Rect;


};

