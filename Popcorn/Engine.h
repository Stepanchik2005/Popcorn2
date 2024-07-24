#pragma once

#include <Windows.h>

#include "Border.h"
#include "Level.h"
#include "Platform.h"
#include "Ball_Set.h"
//------------------------------------------------------------------------------------------------------------
enum EKey_Type
{
	EKT_Left,
	EKT_Right,
	EKT_Space
};
//------------------------------------------------------------------------------------------------------------
enum EGame_State
{
	EGS_Test_Ball,

	EGS_Play_Level,
	EGS_Lost_Ball,
	EGS_Restart_Level
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
	
	double Rest_Distance;
	int Life_Count;
	EGame_State Game_State;
	AsBall_Set Ball_Set;
	AsLaser_Beam_Set Laser_Beam_Set;
	ALevel Level;
	AsPlatform Platform;
	AsBorder Border;

	AMover *Movers[AsConfig::Max_Movers_Count];
	AGraphics_Objects *Graphics_Objects[AsConfig::Max_Graphics_Objects_Count];
};
//------------------------------------------------------------------------------------------------------------
