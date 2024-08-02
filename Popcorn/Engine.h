#pragma once

#include "Border.h"
#include "Level.h"
#include "Platform.h"
#include "Ball_Set.h"
#include "Monster_Set.h"
//------------------------------------------------------------------------------------------------------------
enum class EKey_Type: unsigned char
{
	Left,
	Right,
	Space
};
//------------------------------------------------------------------------------------------------------------
enum class EGame_State: unsigned char
{
	Test_Ball,

	Play_Level,
	Lost_Ball,
	Restart_Level
};
//------------------------------------------------------------------------------------------------------------
const int Timer_ID = WM_USER + 1;

//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();

	void Init_Engine(HWND hwnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key(EKey_Type key_type, bool key_down);
	int On_Timer();

private:
	
	void Advance_Mover();
	void Play_Level();
	void Restart_Level();
	void Act();
	void On_Falling_Letter(AFalling_Letter *falling_letter);
	void Add_New_Module(int &index, AGame_Object *obj);

	double Rest_Distance;
	int Life_Count;

	EGame_State Game_State;
	AsBall_Set Ball_Set;
	AsLaser_Beam_Set Laser_Beam_Set;
	ALevel Level;
	AsPlatform Platform;
	AsBorder Border;
	AsMonster_Set Monster_Set;

	AGame_Object *Modules[AsConfig::Max_Modules_Count];
};
//------------------------------------------------------------------------------------------------------------
