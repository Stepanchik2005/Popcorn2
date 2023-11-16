#include "Active_Brick.h"

AActive_Brick::AActive_Brick(EBrick_Type brick_type)
 : Brick_Type(brick_type),Fade_Step(0), pen(0) , brush(0), brick_rect{}
{
}

HPEN AActive_Brick::Fading_Blue_Bricks_Pen[Max_Fade_Step];
HBRUSH AActive_Brick::Fading_Blue_Bricks_Brushes[Max_Fade_Step];
HPEN AActive_Brick::Fading_Red_Bricks_Pen[Max_Fade_Step];
HBRUSH AActive_Brick::Fading_Red_Bricks_Brushes[Max_Fade_Step];
void AActive_Brick::Act()
{
   if(Fade_Step < Max_Fade_Step - 1)
   {
     ++Fade_Step;
     InvalidateRect(AsConfig::Hwnd, &brick_rect, FALSE);
   }
}

 void AActive_Brick::Draw(HDC hdc,RECT &paint_area)
 {
  
   switch(Brick_Type)
   {
   case EBT_Blue:
      pen = Fading_Blue_Bricks_Pen[Fade_Step];
      brush = Fading_Blue_Bricks_Brushes[Fade_Step];
      break;
   
   case EBT_Red:
      pen = Fading_Red_Bricks_Pen[Fade_Step];
      brush = Fading_Red_Bricks_Brushes[Fade_Step];
      break;
   }
   SelectObject(hdc, pen);
   SelectObject(hdc, brush);

   brick_rect.left = (AsConfig::Level_X_Offset +  AsConfig::Cell_Wight) * AsConfig::Global_Scale;
   brick_rect.top = (AsConfig::Level_Y_Offset + AsConfig::Cell_Height) * AsConfig::Global_Scale;
   brick_rect.right =  brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
   brick_rect.bottom = brick_rect.top  + AsConfig::Brick_Height * AsConfig::Global_Scale;

   RoundRect(hdc, brick_rect.left, brick_rect.top ,brick_rect.right, brick_rect.bottom, 2 * AsConfig::Global_Scale, 2 * AsConfig::Global_Scale);


 }
void AActive_Brick::Setup_Colors()
{
     int i;
     int max_fade_step = Max_Fade_Step - 1;
     for(i = 0; i < Max_Fade_Step; i++)
     {
        Get_Fading_Color(AsConfig::Blue_Brick_Color, i, Fading_Blue_Bricks_Pen[i], Fading_Blue_Bricks_Brushes[i]);
        Get_Fading_Color(AsConfig::Red_Brick_Color, i, Fading_Red_Bricks_Pen[i], Fading_Red_Bricks_Brushes[i]);
     } 
}
unsigned char AActive_Brick::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step)
{
    return color - step * (color - bg_color) / (AActive_Brick::Max_Fade_Step - 1);
}
void AActive_Brick::Get_Fading_Color(const AColor &color,int step, HPEN &pen, HBRUSH &brush)
{
    unsigned char r, g, b;

    r = Get_Fading_Channel(color.R, AsConfig::BG_Color.R, step);
    g = Get_Fading_Channel(color.G, AsConfig::BG_Color.G, step);
    b = Get_Fading_Channel(color.B, AsConfig::BG_Color.B, step);

    AsConfig::Create_Pen_Brush(r, g, b, pen, brush);
}
  