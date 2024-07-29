#pragma once

#include "Ball.h"

class AGate: public AGraphics_Objects
{
public:
	AGate(int x_pos, int y_pos);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);


	static const int Gates_Jumper_Count = 9;
	
private:
	int X_Pos, Y_Pos;
	int Height;

	void Draw_Cup(HDC hdc, RECT &paint_area, bool is_top_cup);
	void Draw_Edges(HDC hdc, RECT &paint_area);
	void Draw_Edge(HDC hdc, RECT &paint_area, int x, int y, bool is_blue);
};
//------------------------------------------------------------------------------------------------------------
class AsBorder: public AHit_Checker, public AGraphics_Objects
{
public:
	~AsBorder();
	AsBorder();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

   void Redraw_Floor();
private:
	void Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border);
	void Draw_Floor(HDC hdc, RECT &paint_area);


	
	RECT Floor_Rect;
	AGate *Gate[8];
};
//------------------------------------------------------------------------------------------------------------
