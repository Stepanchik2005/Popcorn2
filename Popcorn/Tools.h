#pragma once
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
enum class EBall_State: unsigned char
{
	Disabled,

	Normal,
	Lost,
	On_Platform,
	On_Paraschute, 
	Off_Paraschute,
	Teleporting
};


class ABall_Object
{
public:
	virtual double Get_Direction() = 0;
	virtual void Set_Direction(double new_direction) = 0;
	virtual bool Is_Moving_Up() = 0;
	virtual EBall_State Get_State() = 0;
	virtual void Set_State(EBall_State new_state, double x_pos = 0, double y_pos = 0) = 0;
	virtual void Reflect(bool from_horizontal) = 0;
	virtual void Get_Center(double &x_pos, double &y_pos) = 0;
	virtual bool Is_Moving_Left() = 0;
	virtual void Set_On_Paraschute(int brick_x, int brick_y) = 0;
	virtual void Draw_Teleporting(HDC hdc, int step) = 0;
};
class AHit_Checker
{
public:
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball) = 0;
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);
	bool Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x);
};

//------------------------------------------------------------------------------------------------------------

class AGraphics_Objects
{
public:
	virtual ~AGraphics_Objects();
	virtual void Act() = 0;
	virtual bool Is_Finished() = 0;
	virtual void Draw(HDC hdc, RECT &paint_area) = 0;
	virtual void Clear(HDC hdc, RECT &paint_area) = 0;
};

class AMover
{
public:
	virtual ~AMover();
	virtual void Advance(double max_speed) = 0;
	virtual double Get_Speed() = 0;
	virtual void Start_Movement() = 0;
	virtual void End_Movement() = 0;
	
};

class AGame_Object: public AMover, public AGraphics_Objects
{

};

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	//~AColor();
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor &color,int pen_size);
	AColor(const AColor &pen_color, int pen_size, const AColor &brush_color);
	AColor(unsigned char r, unsigned char g, unsigned char b,int pen_size);

	int Get_RGB() const;
	void Select(HDC hdc) const;
	HBRUSH Get_Brush() const;
	HPEN Get_Pen() const;
	void Set_Brush(HBRUSH brush);
	void Set_Pen(HPEN pen);
	void Select_Pen(HDC hdc) const;
	
	unsigned char R, G, B;
private:
	HPEN Pen;
	HBRUSH Brush;
	
};

class AsGame_Objects_Set: public AGame_Object
{
public:
	virtual void Advance(double max_speed);
	virtual double Get_Speed();
   virtual void Start_Movement();
   virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

protected:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **obj) = 0;
};