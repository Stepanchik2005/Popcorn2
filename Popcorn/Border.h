#pragma once

#include "Ball.h"
#include "Gate.h"
//------------------------------------------------------------------------------------------------------------
class AsBorder: public AHit_Checker, public AGame_Object
{
public:
	~AsBorder();
	AsBorder();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	void Open_Gate(int gate_index, bool is_open_short);
   void Redraw_Floor();
	bool Is_Gate_Opened(int gate_index); 
private:
	void Draw_Element(HDC hdc, RECT &paint_area, int x, int y, bool top_border);
	void Draw_Floor(HDC hdc, RECT &paint_area);


	
	RECT Floor_Rect;
	AGate *Gate[8];
};
//------------------------------------------------------------------------------------------------------------
