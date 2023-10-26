#include "Platform.h"
//----------------------------------------------------------------------------
// APlatrorm
AsPlatform::AsPlatform()
 : Inner_width(21), X_Step(AsConfig::Global_Scale), Width(28), X_Pos(AsConfig::Border_X_Offset), Platform_Circle_Pen(0), Platform_Inner_Pen(0), Highlight_Pen(0),
   Platform_Circle_Brush(0), Platform_Inner_Brush(0), Platform_Rect{}, Prev_Platform_Rect{}
{
}

void AsPlatform::Init()
 {
     AsConfig::Create_Pen_Brush(151, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);//
     AsConfig::Create_Pen_Brush(0, 128, 192, Platform_Inner_Pen, Platform_Inner_Brush);//
     Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255)); // белый
}

void AsPlatform::Draw(HDC hdc, HPEN bg_pen, HBRUSH bg_brush, RECT& paint_area)
{
   int x = X_Pos;
   int y = AsConfig::Y_Pos;
         
   RECT intersertion_rect;
    if (! IntersectRect(&intersertion_rect, &paint_area, &Platform_Rect))
         return;

   //перерисовка старой платформы цветом бэкграунда
   SelectObject(hdc, bg_pen);
   SelectObject(hdc, bg_brush);
  // Rectangle(hdc, x * Global_Scale, y * Global_Scale, (x + Width) * Global_Scale, (y + Height) * Global_Scale);
   Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
   //1. рисуем шарики
   SelectObject(hdc, Platform_Circle_Pen);
   SelectObject(hdc, Platform_Circle_Brush);

   Ellipse(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + Circle_size) * AsConfig::Global_Scale, (y + Circle_size) * AsConfig::Global_Scale);
   Ellipse(hdc, (x + Inner_width) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + Circle_size + Inner_width) * AsConfig::Global_Scale, (y + Circle_size) * AsConfig::Global_Scale);

   //2.рисуем блик
   SelectObject(hdc, Highlight_Pen);
   Arc(hdc, (x + 1) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + Circle_size - 1) * AsConfig::Global_Scale, (y + Circle_size - 1) * AsConfig::Global_Scale,
      (x + 1 + 1) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale, (y + 1 + 2) * AsConfig::Global_Scale);
   
   //3. рисуем внутренню часть платформы
   SelectObject(hdc, Platform_Inner_Pen);
   SelectObject(hdc, Platform_Inner_Brush);

   RoundRect(hdc, (x + 4) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4 + Inner_width - 1) * AsConfig::Global_Scale, (y + 1 + 5) * AsConfig::Global_Scale, 3 * AsConfig::Global_Scale, 3 * AsConfig::Global_Scale);
}


void AsPlatform::Redraw_Platform(HWND hwnd)
{// перерисовка платформы
   Prev_Platform_Rect = Platform_Rect;
   
   Platform_Rect.left = X_Pos * AsConfig::Global_Scale;
   Platform_Rect.top = AsConfig::Y_Pos * AsConfig::Global_Scale;
   Platform_Rect.right = Platform_Rect.left + Width * AsConfig::Global_Scale;
   Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

   InvalidateRect(hwnd, &Prev_Platform_Rect, FALSE);

   InvalidateRect(hwnd,&Platform_Rect, FALSE);

}