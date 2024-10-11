#pragma once
#include "Config.h"

class AMop_Cylinder: public AGraphics_Objects
{
public:
	AMop_Cylinder();
	AMop_Cylinder(int x_pos, int y_pos, int width, int max_height);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Set_Height_For(double ratio);
	int Get_Height();
	void Set_Y_Pos(int y_pos);

	static const int Max_Cylinder_Height[4]; 
private: 
	int X_Pos,  Y_Pos;
	int Width, Height;
	int Max_Height;
	RECT Cylinder_Rect, Prev_Cylinder_Rect;
	
	static const int Binding_Height = 4;
	static const int Min_Height = Binding_Height + 1;
};