#pragma once
#include "Config.h"
#include "Level.h"

enum EBall_State
{
   EBS_Normal,
   EBS_Lost,
   EBS_On_Platform
};
class ABall
{
public:
   ABall();
   void Init();
   void Move(ALevel *level, int platform_x_pos, int platform_width);
   void Draw(HDC hdc, RECT &paint_area);
   
   void Set_State(EBall_State new_state, int x_pos);
   EBall_State Get_State();
   double Ball_Direction;

   HPEN Ball_Pen;
   HBRUSH Ball_Brush;
  
private:
   void Redraw_Ball();
   bool Check_Border_Hit(double next_x_pos,double next_y_pos);
   EBall_State Ball_State;
   double Center_X_Pos;
   double Center_Y_Pos;
   double Ball_Speed;

 
   RECT Ball_Rect, Prev_Ball_Rect;

   static const double Start_Ball_Y_Pos;
   static const double Radius;
};
