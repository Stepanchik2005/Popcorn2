#pragma once
#include "Config.h"
#include "Level.h"

class ABall
{
public:
   ABall();
   void Init();
   void Move(HWND hwnd,  ALevel *level, int platform_x_pos, int platform_width);
   void Draw(HDC hdc, RECT &paint_area);

   double Ball_Direction;

   HPEN Ball_Pen;
   HBRUSH Ball_Brush;
private:
   int Ball_X_Pos;
   int Ball_Y_Pos;
   double Ball_Speed;

 
   RECT Ball_Rect, Prev_Ball_Rect;


  
};
