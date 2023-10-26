
#include "Engine.h"

AsEngine::AsEngine()
: BG_Pen(0),BG_Brush(0), Hwnd(0),Level{},Border{}
{
}
    
      

void AsEngine::Init_Engine(HWND hwnd)
{
   Hwnd = hwnd;

   AsConfig::Create_Pen_Brush(15, 63, 31, BG_Pen, BG_Brush);//

   Level.Init();

   Ball.Init();
   Platform.Init();
   Border.Init();
   Platform.Redraw_Platform(hwnd);

   SetTimer(Hwnd, Timer_ID, 50, 0);
}

void AsEngine::Draw_Frame(HDC hdc, RECT& paint_area)
{// рисуем все окно
   /*int i;

   for (i = 0; i < 16; ++i) {
      Draw_Brick_Letter(hdc, 20 + i * Cell_Wight * Global_Scale, 100, EBT_Blue,ELT_O, i);
      Draw_Brick_Letter(hdc, 20 + i * Cell_Wight * Global_Scale, 130, EBT_Red,ELT_O, i);
   }*/

      Level.Draw(hdc, paint_area);
      Platform.Draw(hdc, BG_Pen, BG_Brush, paint_area);

      Ball.Draw(hdc, paint_area, BG_Pen, BG_Brush);

      Border.Draw(hdc, paint_area, BG_Pen,BG_Brush);
}

int AsEngine::On_Key_Down(EKey_Type key_type)
{
   switch (key_type)
   {
   case EKT_Left:
      Platform.X_Pos -= Platform.X_Step;

      if (Platform.X_Pos <= AsConfig::Border_X_Offset)
         Platform.X_Pos = AsConfig::Border_X_Offset;
      Platform.Redraw_Platform(Hwnd);
      break;

   case EKT_Right:
      Platform.X_Pos += Platform.X_Step;

      if (Platform.X_Pos >= AsConfig::Max_X_Pos - Platform.Width + 1)
         Platform.X_Pos = AsConfig::Max_X_Pos - Platform.Width + 1;
      Platform.Redraw_Platform(Hwnd);

      break;

   case EKT_Space:
       break;
   }

   return 0;
}

int AsEngine::On_Timer()
{
   Ball.Move(Hwnd, &Level, Platform.X_Pos, Platform.Width);
   return 0;
}

















