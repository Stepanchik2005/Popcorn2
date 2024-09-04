#include "Laser_Beam.h"



// ALaser_Beam

AHit_Checker_List ALaser_Beam::Hit_Checker_List = {};

ALaser_Beam::ALaser_Beam()
   : Laser_Beam_State(Elaser_Beam_State::Disabled),Speed(0.0), X_Pos(0.0), Y_Pos(0.0),Prev_Beam_Rect({}), Beam_Rect({})
{
}

void ALaser_Beam::Advance(double max_speed)
{
   double next_step;
   bool got_hit = false;

   if(Laser_Beam_State != Elaser_Beam_State::Active)
      return;

   next_step = Speed / max_speed * AsConfig::Moving_Size_Step;

   Y_Pos -= next_step;

   // проверяем попадание в хит чекер
   got_hit = Hit_Checker_List.Check_Hit(X_Pos, Y_Pos);

   if(got_hit)
   {
     Laser_Beam_State = Elaser_Beam_State::Stopping;
     Speed = 0.0;
   }
}
double ALaser_Beam::Get_Speed()
{
   return Speed;
}
void ALaser_Beam::Start_Movement()
{
   // заглушка этот метод здесь не используется
}
void ALaser_Beam::End_Movement()
{
   if(Laser_Beam_State != Elaser_Beam_State::Disabled)
         Redraw_Beam();
}

void ALaser_Beam::Act()
{
   // заглушка т.к. луч не имеет своей анимации
}
bool ALaser_Beam::Is_Finished()
{
   return false; // заглушка т.к. луч не имеет своей анимации
}
void ALaser_Beam::Draw(HDC hdc, RECT &paint_area)
{
   const double d_scale = AsConfig::D_Global_Scale;
   double x_pos = X_Pos * d_scale;
   double y_pos = Y_Pos * d_scale;
   RECT intersection_rect = {};
   
   if(Laser_Beam_State != Elaser_Beam_State::Active)
      return;

   if(Laser_Beam_State == Elaser_Beam_State::Stopping)
      Laser_Beam_State = Elaser_Beam_State::Cleanup;

   if (IntersectRect(&intersection_rect, &paint_area, &Beam_Rect) )
   {
      AsConfig::Laser_Color.Select(hdc);
      MoveToEx(hdc, (int) x_pos,(int) y_pos + 1, 0);
      LineTo(hdc, (int) x_pos, (int) y_pos + Height * AsConfig::Global_Scale - 1 - 1);
   }
}
void ALaser_Beam::Clear(HDC hdc, RECT &paint_area)
{
   RECT intersection_rect;

   if(Laser_Beam_State == Elaser_Beam_State::Disabled)
      return;

   if(Laser_Beam_State == Elaser_Beam_State::Cleanup)
      Laser_Beam_State = Elaser_Beam_State::Disabled;

   if(!IntersectRect(&intersection_rect, &paint_area, &Prev_Beam_Rect))
      return;

   AsCommon::Rect(hdc, Prev_Beam_Rect, AsConfig::BG_Color);

}
void ALaser_Beam::Set_At(double x_pos, double y_pos)
{
   X_Pos = x_pos;
   Y_Pos = y_pos;

   Laser_Beam_State = Elaser_Beam_State::Active;
   Speed = 3.0;
}
bool ALaser_Beam::Is_Active()
{
   if(Laser_Beam_State == Elaser_Beam_State::Active)
      return true;
   else
      return false;
}

void ALaser_Beam::Disable()
{
   Laser_Beam_State = Elaser_Beam_State::Stopping;
   Speed = 0.0;
}

void ALaser_Beam::Redraw_Beam()
{
   Prev_Beam_Rect = Beam_Rect;

   Beam_Rect.left = (int)( (X_Pos - (double)Width / 2.0) * AsConfig::D_Global_Scale );
   Beam_Rect.top = (int)(Y_Pos * AsConfig::D_Global_Scale);
   Beam_Rect.right = Beam_Rect.left + Width * AsConfig::Global_Scale;
   Beam_Rect.bottom = Beam_Rect.top + Height * AsConfig::Global_Scale;

   AsCommon::Invalidate_Rect(Prev_Beam_Rect);
   AsCommon::Invalidate_Rect(Beam_Rect);
}
