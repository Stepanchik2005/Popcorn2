
#include "Engine.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------

AsEngine::AsEngine()
: Level{},Border{}, Game_State(EGS_Play_Level)
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
    
void AsEngine::Init_Engine(HWND hwnd)
{
   AsConfig::Hwnd = hwnd;

   AActive_Brick::Setup_Colors();

   Level.Init();

   
   Platform.Init();
   Ball.Init();
   Border.Init();
   Platform.Redraw_Platform();

   Ball.Set_State(EBS_Normal, Platform.X_Pos + Platform.Width / 2);
   Platform.Setup_State(EPS_Normal);

   SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

void AsEngine::Draw_Frame(HDC hdc, RECT& paint_area)
{// рисуем все окно
   /*int i;

   for (i = 0; i < 16; ++i) {
      Draw_Brick_Letter(hdc, 20 + i * Cell_Wight * Global_Scale, 100, EBT_Blue,ELT_O, i);
      Draw_Brick_Letter(hdc, 20 + i * Cell_Wight * Global_Scale, 130, EBT_Red,ELT_O, i);
   }*/

      Level.Draw(hdc, paint_area);
     
      Ball.Draw(hdc, paint_area);
      Platform.Draw(hdc, paint_area);
      Border.Draw(hdc, paint_area);
     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

int AsEngine::On_Key_Down(EKey_Type key_type)
{
   switch (key_type)
   {
   case EKT_Left:
      Platform.X_Pos -= Platform.X_Step;

      if (Platform.X_Pos <= AsConfig::Border_X_Offset)
         Platform.X_Pos = AsConfig::Border_X_Offset;
      Platform.Redraw_Platform();
      break;

   case EKT_Right:
      Platform.X_Pos += Platform.X_Step;

      if (Platform.X_Pos >= AsConfig::Max_X_Pos - Platform.Width + 1)
         Platform.X_Pos = AsConfig::Max_X_Pos - Platform.Width + 1;
      Platform.Redraw_Platform();

      break;

   case EKT_Space:
      if(Platform.Get_State() == EPS_Ready)
      {
         Ball.Set_State(EBS_Normal, Platform.X_Pos + Platform.Width / 2);
         Game_State = EGS_Play_Level;
         Platform.Setup_State(EPS_Normal);
      }
       break;
   }

   return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

int AsEngine::On_Timer()
{
   ++AsConfig::Current_Time_Tick;
  
  // Level.Active_Brick.Act();

   switch(Game_State)
   {
   case EGS_Play_Level:
      Ball.Move(&Level, Platform.X_Pos, Platform.Width);

      if(Ball.Get_State() == EBS_Lost)
      {
         Game_State = EGS_Lost_Ball;
         Platform.Setup_State(EPS_Meltdown);
      }

      break;
   
   case EGS_Lost_Ball:
       if(Platform.Get_State() == EPS_Missing)
       {
         Game_State = EGS_Restart_Level;
         Platform.Setup_State(EPS_Roll_In);
       }
       break;   
    
   case EGS_Restart_Level:
       if(Platform.Get_State() == EPS_Ready)
       {
          Game_State = EGS_Play_Level;
          Ball.Set_State(EBS_On_Platform, Platform.X_Pos + Platform.Width / 2);
         
       }
       break;

   }
   //if(AsConfig::Current_Time_Tick % 10 == 0)
   Platform.Act();
   return 0;
}

















