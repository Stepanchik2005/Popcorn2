#include "Active_Brick.h"

AActive_Brick::AActive_Brick()
 : Fade_Step(0), pen(0) , brush(0), brick_rect{}
{
}

void AActive_Brick::Act(HWND hwnd)
{
   if(Fade_Step < 40)
   {
     ++Fade_Step;
     InvalidateRect(hwnd, &brick_rect, FALSE);
   }
}

 void AActive_Brick::Draw(HWND hwnd, HDC hdc,RECT &paint_area)
 {
 

   AsConfig::Create_Pen_Brush(85 - Fade_Step * (85 / Max_Fade_Step), 255 - Fade_Step * (255 / Max_Fade_Step), 255 - Fade_Step * (255 / Max_Fade_Step), pen, brush);
   SelectObject(hdc, pen);
   SelectObject(hdc, brush);

   brick_rect.left = (AsConfig::Level_X_Offset +  AsConfig::Cell_Wight) * AsConfig::Global_Scale;
   brick_rect.top = (AsConfig::Level_Y_Offset + AsConfig::Cell_Height) * AsConfig::Global_Scale;
   brick_rect.right =  brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
   brick_rect.bottom = brick_rect.top  + AsConfig::Brick_Height * AsConfig::Global_Scale;

   RoundRect(hdc, brick_rect.left, brick_rect.top ,brick_rect.right, brick_rect.bottom, 2 * AsConfig::Global_Scale, 2 * AsConfig::Global_Scale);


 }
