#include "Border.h"


//AsBorder----------------------------------------------------------------------------
AsBorder::AsBorder()
   : Border_Blue_Pen(0), Border_White_Pen(0), Border_Blue_Brush(0), Border_White_Brush(0)
{    
}
void AsBorder::Init()
{
   AsConfig::Create_Pen_Brush(85, 225, 255, Border_Blue_Pen, Border_Blue_Brush);
   AsConfig::Create_Pen_Brush(255, 255, 255, Border_White_Pen, Border_White_Brush);// белый
}
  
void AsBorder::Draw(HDC hdc, RECT& paint_area, HPEN bg_pen, HBRUSH bg_brush)
{// рисуем все рамки
   int i;
   //рисуем левую рамку
   for (i = 0; i < 50; i++)
      Draw_Element(hdc, 2, 1 + i * 4, false, bg_pen,bg_brush);
   //рисуем правую рамку
   for (i = 0; i < 50; i++)
      Draw_Element(hdc, 201, 1 + i * 4, false,  bg_pen,bg_brush);
   //рисуем верхнюю рамку
   for (i = 0; i < 50; i++)
     Draw_Element(hdc, 3 + i * 4, 0, true,  bg_pen,bg_brush);
}


void AsBorder::Draw_Element(HDC hdc, int x, int y, bool top_border, HPEN bg_pen, HBRUSH bg_brush)
{//рисуем элемент рамки
   //голубая часть рамки
   SelectObject(hdc, Border_Blue_Pen);
   SelectObject(hdc, Border_Blue_Brush);

   if (top_border) 
      Rectangle(hdc, x * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale, (y + 4) * AsConfig::Global_Scale);
   else
      Rectangle(hdc, (x + 1) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale, (y + 4) * AsConfig::Global_Scale);

   //белая часть рамки
   SelectObject(hdc, Border_White_Pen);
   SelectObject(hdc, Border_White_Brush);

   if (top_border)
      Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale);
   else
      Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale, (y + 4) * AsConfig::Global_Scale);
   // фоновая часть рамки
   SelectObject(hdc, bg_pen);
   SelectObject(hdc, bg_brush);


   if (top_border)
      Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale);
   else
      Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale);
}

