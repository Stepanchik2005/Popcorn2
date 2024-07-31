#pragma once

#include "Ball.h"

//------------------------------------------------------------------------------------------------------------
enum class EBrick_Type: unsigned char
{
	None,


	Red,
	Blue,
	Unbreakable,
	Multihit_1,
	Multihit_2,
	Multihit_3,
	Multihit_4,
	Paraschute,
	Teleport,
	Ad, 
	Invisible
};
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
class AActive_Brick : public AGraphics_Objects
{
public:
	void Get_X_Y(int &x_pos, int &y_pos);
protected:
	AActive_Brick(EBrick_Type brick_type, int level_x, int level_y);
	
	virtual void Clear(HDC hdc, RECT &paint_area);

	int Level_X, Level_Y; // индексы активного кирпича в двумерном массиве уровня
	EBrick_Type Brick_Type;
	RECT Brick_Rect;
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick_Red_Blue : public AActive_Brick
{
public:
	~AActive_Brick_Red_Blue();

	AActive_Brick_Red_Blue(int level_x, int level_y,EBrick_Type brick_type);

	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();

	static void Setup_Colors();
	static void Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type);
	
private:
	int Fade_Step;


	static unsigned char Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step);
	static void Get_Fading_Color(const AColor &color, int step, AColor &result_color);

	static const int Max_Fade_Step = AsConfig::FPS;

	static AColor Fading_Blue_Colors[Max_Fade_Step];	
	static AColor Fading_Red_Colors[Max_Fade_Step];	
};
//------------------------------------------------------------------------------------------------------------




class AActive_Brick_Unbreakable : public AActive_Brick
{
public:
	~AActive_Brick_Unbreakable();

	AActive_Brick_Unbreakable(int level_x, int level_y);

	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();

	static void Draw_In_Level(HDC hdc, RECT &brick_rect);
private:

	int Animation_Step;
	HRGN Region;

	static const int Max_Animation_Step = 12;
};
//------------------------------------------------------------------------------------------------------------



class AActive_Brick_Multihit : public AActive_Brick
{
public:
	~AActive_Brick_Multihit();

	AActive_Brick_Multihit(int level_x, int level_y);

	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();

	static void Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type);
private:
	static void Draw_Stage(HDC hdc, RECT &brick_rect, int x, int width);

	int Rotation_Step;
   static const int Steps_Per_Turn = 16;
	static const int Max_Rotation_Step = Steps_Per_Turn * 4;
};



//------------------------------------------------------------------------------------------------------------
enum class ETeleport_State: unsigned char
{
	
	Starting,
	Finishing,
	Done
};
enum class EDirection_State: unsigned char
{
	Left,
	Up,
	Right,
	Down
};
class AActive_Brick_Teleport : public AActive_Brick
{
public:
	~AActive_Brick_Teleport();

	AActive_Brick_Teleport(int level_x, int level_y, ABall *ball, AActive_Brick_Teleport *destination_teleport);

	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();

	void Set_Ball(ABall *ball);
	double Get_X_Teleport_Pos(bool is_center);
	double Get_Y_Teleport_Pos(bool is_center);
	static void Draw_In_Level(HDC hdc, RECT &brick_rect, int step = 0);

	EDirection_State Release_Direction_State;
private:
	
	ETeleport_State Teleport_State;
	ABall *Ball;
	AActive_Brick_Teleport *Destination_Teleport;
	int Animation_Step;
 
	static const int Max_Animation_Step = 4;
};




//-----AAdversiment-------------------------------------------------------------------------------------------------------
class AAdvertisement : public AGraphics_Objects
{
public:
	~AAdvertisement();

	AAdvertisement(int level_x, int level_y, int width, int height);

	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Show_Under_Brick(int level_x, int level_y);
	bool Has_Brick_At(int level_x, int level_y);
	RECT Ad_Rect;
	HRGN *Brick_Regions;
   int Max_Brick_Mask_Size;
private:
   int Level_X, Level_Y;
	int Width, Height;
	HRGN Empty_Region;
	int Ball_X, Ball_Y;
	int Ball_Width, Ball_Height;
	double Falling_Speed, Acceleration_Step;
	double Ball_Offset;
	double Deformation_Ratio;
	static const int High_Limit_Animation = 12 * AsConfig::Global_Scale;
	static const int Low_Limit_Animation = -3 * AsConfig::Global_Scale;
	static const int Deformation_Height = 2 * AsConfig::Global_Scale;
	static const int Circle_Size = 12;
};



//-----AActive_Brick_Ad-------------------------------------------------------------------------------------------------------
class AActive_Brick_Ad : public AActive_Brick
{
public:
	~AActive_Brick_Ad();

	AActive_Brick_Ad(int level_x, int level_y, AAdvertisement *advertisement);

	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();
	

	static void Draw_In_Level(HDC hdc, RECT &brick_rect);
private:
	AAdvertisement *Advertisement;


	static const int Circle_Size = 7;
	
};
