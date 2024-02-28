#pragma once

#include "Ball.h"
#include "Falling_Letter.h"
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
struct SPoint
{
	int X, Y;
};
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
class ALevel: public AHit_Checker
{
public:
	~ALevel();
	ALevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	void Init();
	void Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width]);
	void Draw(HDC hdc, RECT &paint_area);
	void Act();
   bool Get_Next_Falling_Letter(int& index, AFalling_Letter **falling_letter);

	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Test_Level[AsConfig::Level_Height][AsConfig::Level_Width];

private:
	bool On_Hit(int brick_x, int brick_y, ABall *ball, bool got_vertical_hit);
	void Clear_Objects(HDC hdc, RECT &paint_area, AGraphics_Objects** object_for_drawing,  int max_size);
	
	void Redraw_Brick(int brick_x, int brick_y);
	bool Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos);
	bool Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos);
	bool Add_Falling_Letter(EBrick_Type brick_type, int brick_x, int brick_y);
	bool Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall *ball, bool got_vertical_hit);
	void Add_Active_Brick(AActive_Brick *active_brick);
	void Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall *ball, bool got_vertical_hit);
	void Act_Objects(AGraphics_Objects** object_for_drawing, int &object_count, int max_size);
	AActive_Brick_Teleport *Select_Destination_Teleport(int brick_x, int brick_y);
	void Draw_Brick(HDC hdc, RECT brick_rect, int level_x, int level_y);
	void Draw_Objects(HDC hdc, RECT &paint_area, AGraphics_Objects** object_for_drawing,  int max_size);
	void Draw_Paraschute_In_Level(HDC hdc, RECT &brick_rect);
	void Draw_Part_Paraschute(HDC hdc, RECT &brick_rect, double offet, int width);

	RECT Level_Rect;	
	

	int Active_Brick_Counter;
	int Falling_Letters_Counter;
	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;
   int Teleport_Counter;
	static const int Max_Teleports_In_Level = 6;
	static const int Min_Teleports_In_Level = 2;
	SPoint *Teleport_Pos;
	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	AActive_Brick *Active_Bricks[AsConfig::Max_Active_Brick_Size];
	AFalling_Letter *Falling_Letters[AsConfig::Max_Falling_Letters_Size];
	AAdvertisement *Advertisement;
};
//------------------------------------------------------------------------------------------------------------
