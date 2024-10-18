#pragma once
#include "Falling_Letter.h"
#include "Info_Panel.h"
#include "Level_Data.h"
#include "Mop.h"
#include "Final_Title.h"
#include "Level_Title.h"
//------------------------------------------------------------------------------------------------------------
class APoint
{
public:
	APoint();
	APoint(int x_pos, int y_pos);

	int X, Y;
};



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
	void Set_Current_Level(int level_index);
   bool Get_Next_Falling_Letter(int& index, AFalling_Letter **falling_letter);
	void Stop();
	void Mop_Level(int next_level);
	void Mop_Next_Level();
	bool Is_Mopping_Done();
	bool Can_Mop_Next_Level();

   void Show_Level_Table();
	void Hide_Level_Table();

	static bool Has_Brick_At(int brick_x, int brick_y);
	static bool Has_Brick_At(RECT &monster_rect);

	int Next_Level_Number, Current_Level_Number;
	int Active_Brick_Count;

	
	AsMop Mop;
	AsFinal_Title Final_Title;

private:
	bool On_Hit(int brick_x, int brick_y, ABall_Object *ball, bool got_vertical_hit);
	void Clear_Objects(HDC hdc, RECT &paint_area, std::vector<AGraphics_Objects *> &object_for_drawing);
	
	void Redraw_Brick(int brick_x, int brick_y);
	bool Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos);
	bool Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos);
	bool Add_Falling_Letter(EBrick_Type brick_type, int brick_x, int brick_y);
	bool Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall_Object *ball, bool got_vertical_hit);
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
	AsLevel_Title Level_Title;
	

	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;

	
	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	std::vector<AGraphics_Objects *>Active_Bricks;
	std::vector<AGraphics_Objects *>Falling_Letters;
	std::vector<APoint> Teleport_Pos;
	std::vector<ALevel_Data *> Levels_Data;
	AAdvertisement *Advertisement;
	bool Need_To_Stop_All_Activity;

	
	static ALevel *Level;

	static const int Max_Teleports_In_Level = 6;
	static const int Min_Teleports_In_Level = 2;
};
//------------------------------------------------------------------------------------------------------------
