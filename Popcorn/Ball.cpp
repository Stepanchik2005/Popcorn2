#include "Ball.h"
//--------------------------------------------------------------------------------------------------------------------------

  const double ABall::Start_Ball_Y_Pos = 181.0;
  const double ABall::Radius = 2.0;
ABall::ABall()
    :  Ball_State(EBS_Normal), Center_X_Pos(0), Center_Y_Pos(Start_Ball_Y_Pos), Ball_Speed(0), Ball_Direction(0), Ball_Pen(0), Ball_Brush(0), Ball_Rect{}, Prev_Ball_Rect{}
 {
   Set_State(EBS_Normal, 0);
 }
//------------------------------------------------------------------------------------------------------------------------------------------------------

 void ABall:: Init()
 {
    AsConfig::Create_Pen_Brush(255, 255, 255, Ball_Pen, Ball_Brush);

 }
 
 //------------------------------------------------------------------------------------------------------------------------------------------------------

 void ABall::Move(ALevel *level, int platform_x_pos, int platform_width)
{
   double next_x_pos, next_y_pos;
   bool got_hit;
   int platform_y_pos = AsConfig::Y_Pos - AsConfig::Ball_Size;
  
   double step_size = 1.0 / AsConfig::Global_Scale;
   double rest_distance = Ball_Speed;

   if(Ball_State != EBS_Normal)
      return;

   Prev_Ball_Rect = Ball_Rect;

   while(rest_distance >= step_size)
   {
      next_x_pos = Center_X_Pos + step_size * cos(Ball_Direction);
      next_y_pos = Center_Y_Pos - step_size * sin(Ball_Direction);
      got_hit = Check_Border_Hit(next_x_pos,next_y_pos);
      
      //������������ ��������� ������ �� ��������
      //if (next_y_pos > platform_y_pos) 
      //{
      //   if (next_x_pos >= platform_x_pos and next_x_pos <= platform_x_pos + platform_width)
      //   {
      //      next_y_pos = platform_y_pos - (next_y_pos - platform_y_pos);
      //      Ball_Direction = M_PI + (M_PI - Ball_Direction);
      //   }
      //}

      ////удар об кирпич 
      //level->Check_Level_Brick_Hit(next_y_pos, Ball_Direction);
       
      if(! got_hit)
      {
        rest_distance -= step_size;
        
        Center_X_Pos = next_x_pos;
        Center_Y_Pos = next_y_pos;
      }
      
   }
   Redraw_Ball();
   
}
 //------------------------------------------------------------------------------------------------------------------------------------------------------

 
void ABall::Draw(HDC hdc, RECT& paint_area)
{
    RECT intersertion_rect;
    if (IntersectRect(&intersertion_rect, &paint_area, &Prev_Ball_Rect))
    {
     // 1. перерисовываем старое место шарика
          SelectObject(hdc, AsConfig::BG_Pen);
          SelectObject(hdc, AsConfig::BG_Brush);

          Ellipse(hdc, Prev_Ball_Rect.left , Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);
    }

    if (IntersectRect(&intersertion_rect, &paint_area, &Ball_Rect))
    {
       SelectObject(hdc, Ball_Pen);
       SelectObject(hdc, Ball_Brush);
       
       // рисуем новый шарик 
       Ellipse(hdc, Ball_Rect.left, Ball_Rect.top , Ball_Rect.right - 1, Ball_Rect.bottom - 1);
    }
}

EBall_State ABall::Get_State()
{
   return Ball_State;
}
void ABall::Set_State(EBall_State new_state, int x_pos)
{
   switch(new_state)
   {
   case EBS_Normal:
     
      Center_X_Pos = (double)x_pos;
      Center_Y_Pos = Start_Ball_Y_Pos;
      Ball_Speed = 3.0;
      Ball_Direction = M_PI - M_PI_4;
      Redraw_Ball();
      break;

   case EBS_Lost:
      Ball_Speed = 0.0;
      break;

   case EBS_On_Platform:
      Center_X_Pos = (double)x_pos;
      Center_Y_Pos = Start_Ball_Y_Pos;
      Ball_Speed = 0.0;
      Ball_Direction = M_PI - M_PI_4;
      Redraw_Ball();
        break;

     
   }
    Ball_State = new_state;
}
void ABall::Redraw_Ball()
 {
  
   Ball_Rect.left = (int)((Center_X_Pos - Radius) * AsConfig::Global_Scale);
   Ball_Rect.top = (int)((Center_Y_Pos - Radius)* AsConfig::Global_Scale);
   Ball_Rect.right = (int)((Center_X_Pos + Radius))* AsConfig::Global_Scale;
   Ball_Rect.bottom = (int)((Center_Y_Pos + Radius))* AsConfig::Global_Scale;

   InvalidateRect(AsConfig::Hwnd, &Prev_Ball_Rect, FALSE);
   InvalidateRect(AsConfig::Hwnd, &Ball_Rect, FALSE);
 }
bool ABall::Check_Border_Hit(double next_x_pos,double next_y_pos)
{
   bool got_hit = false;
   //задавание след позиции шарика
     
      //отражение шарика от границ
      if (next_x_pos - Radius < AsConfig::Border_X_Offset) 
      {
         got_hit = true;
         Ball_Direction = M_PI - Ball_Direction;
      }
      
      if (next_y_pos - Radius < AsConfig::Border_Y_Offset)
      {
         got_hit = true;
         Ball_Direction = -Ball_Direction;   
      }

      if (next_x_pos + Radius > AsConfig::Max_X_Pos) 
      {
          got_hit = true;
         Ball_Direction = M_PI - Ball_Direction;
      }
      if (next_y_pos + Radius > AsConfig::Max_Y_Pos) 
      {
         if(AsConfig::Level_Has_Floor)
         {
            got_hit = true;
            
            Ball_Direction = - Ball_Direction;
         }
         else
         {
            if(next_y_pos + Radius > AsConfig::Max_Y_Pos + + Radius * 4.0)
               Ball_State = EBS_Lost;
         }
      }
      return got_hit;
}
