#pragma once

#include "Falling_Letter.h"
#include "Info_Panel.h"
//------------------------------------------------------------------------------------------------------------
struct SPoint
{
	int X, Y;
};
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
class ALevel: public AHit_Checker, public AGame_Object
{
public:
	~ALevel();
	ALevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	void Init();
	void Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width]);
   bool Get_Next_Falling_Letter(int& index, AFalling_Letter **falling_letter);
	void Stop();

	static bool Has_Brick_At(int brick_x, int brick_y);
	static bool Has_Brick_At(RECT &monster_rect);

	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Test_Level[AsConfig::Level_Height][AsConfig::Level_Width];

private:
	bool On_Hit(int brick_x, int brick_y, ABall_Object *ball, bool got_vertical_hit);
	void Clear_Objects(HDC hdc, RECT &paint_area, std::vector<AGraphics_Objects *> &object_for_drawing);
	
	void Redraw_Brick(int brick_x, int brick_y);
	bool Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos);
	bool Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos);
	bool Add_Falling_Letter(EBrick_Type brick_type, int brick_x, int brick_y);
	bool Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall_Object *ball, bool got_vertical_hit);
	//void Add_Active_Brick(AActive_Brick *active_brick);
	void Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall_Object *ball, bool got_vertical_hit);
	void Act_Objects(std::vector<AGraphics_Objects *> &object_for_drawing);
	AActive_Brick_Teleport *Select_Destination_Teleport(int brick_x, int brick_y);
	void Draw_Brick(HDC hdc, RECT brick_rect, int level_x, int level_y);
	void Draw_Objects(HDC hdc, RECT &paint_area, std::vector<AGraphics_Objects *> &object_for_drawing);
	void Draw_Paraschute_In_Level(HDC hdc, RECT &brick_rect);
	void Draw_Part_Paraschute(HDC hdc, RECT &brick_rect, double offet, int width);
	void Delete();
	void Delete_Objects(std::vector<AGraphics_Objects *> &object_for_drawing);
	RECT Level_Rect;	
	
	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;
   int Teleport_Counter;
	static const int Max_Teleports_In_Level = 6;
	static const int Min_Teleports_In_Level = 2;
	SPoint *Teleport_Pos;
	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	std::vector<AGraphics_Objects *>Active_Bricks;
	std::vector<AGraphics_Objects *>Falling_Letters;

	AAdvertisement *Advertisement;
	bool Need_To_Stop_All_Activity;
	static ALevel *Level;
};
//------------------------------------------------------------------------------------------------------------
