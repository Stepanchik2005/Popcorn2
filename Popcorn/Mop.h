#pragma once
#include "Mop_Indicator.h"
#include "Mop_Cylinder.h"

enum class EMop_State
{
	Idle,

	Ascending,
	Ascending_Done,
	Cleaning,
	Cleaning_Done,
	Showing, 
	Showing_Done,
	Descending,
	Descending_Done
};
class AsMop: public AGame_Object
{
public:
	~AsMop();
	AsMop();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Erase_Level();
	void Set_Mop();
	bool Is_Level_Cleaning_Done();
	EMop_State Get_State();

	static void Init();

private:
	void Clean_Area(HDC hdc);
	int Get_Total_Cylinder_Height();
	bool Act_For_Executing_State(int timeout);
	bool Act_For_Done_State();

	std::vector<AMop_Indicator *> Mop_Indicators;
	std::vector<AMop_Cylinder *> Mop_Cylinders;

	int Y_Pos;
	int Start_Tick;
	int End_Tick;
	int Max_Y_Pos;
	int Lifting_Height;
	
	RECT Mop_Rect, Prev_Mop_Rect;
	EMop_State Mop_State;

	static const int Max_Indicators_Count = 10;
	static const int Width = (AsConfig::Level_Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width;
	static const int Height = AsConfig::Brick_Height;
	static const int Expansion_Timeout = AsConfig::FPS * 2;
	static const int Cleaning_Done_Timeout = AsConfig::FPS; // состояние полностью вытянутой швабры 1 секунда
	static const int Lifting_Timeout = AsConfig::FPS;
	static const int Descending_Timeout = AsConfig::FPS;
};