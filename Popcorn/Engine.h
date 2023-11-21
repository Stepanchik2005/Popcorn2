#pragma once
#include <Windows.h>
#include "Border.h"
#include "Ball.h"
#include "Platform.h"
enum EKey_Type
{
   EKT_Left,
   EKT_Right,
   EKT_Space
};

enum EGame_State
{
   EGS_Play_Level,
   EGS_Lost_Ball,
   EGS_Restart_Level
};
//----------------------//----------------------//----------------------
const int Timer_ID = WM_USER + 1;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class AsEngine
{
public:
   AsEngine();

   void Init_Engine(HWND hwnd);
   void Draw_Frame(HDC hdc,RECT &paint_area);
   int On_Key_Down(EKey_Type key_type);
   int On_Timer();

private:
   EGame_State Game_State;
   ABall Ball;
   ALevel Level;
   AsPlatform Platform;
   AsBorder Border;

};
