#include "Platform_Expanding.h"

// AsPlatform_Expanding
const double AsPlatform_Expanding::Max_Expanding_Size = 40.0; 
const double AsPlatform_Expanding::Min_Expanding_Size = (double)AsConfig::Platform_Normal_Width;
const double AsPlatform_Expanding::Expanding_Platform_Step = 1.0;
//------------------------------------------------------------------------------------------------------------
AsPlatform_Expanding::~AsPlatform_Expanding()
{
   delete Truss_Color;
}
//------------------------------------------------------------------------------------------------------------

AsPlatform_Expanding::AsPlatform_Expanding(AsPlatform_State &platform_state)
   : Expanding_Platform_Width(0), Platform_State(&platform_state), Inner_Color(0), Circle_Color(0), Truss_Color(0), Highlight_Color(0)
{
}
void AsPlatform_Expanding::Init(AColor &inner_color, AColor &circle_color, AColor &highlight_color)
{
   Inner_Color = &inner_color;
   Circle_Color = &circle_color;
   Highlight_Color = &highlight_color;
   Truss_Color = new AColor(inner_color, AsConfig::Global_Scale);
}
bool AsPlatform_Expanding::Act_For_Expanding_State(double &x_pos, bool &is_correct, EPlatform_State &next_state)
{
   is_correct = false;

   switch(Platform_State->Expanding)
   {
   case EPlatform_Transformation::Init:
      if(Expanding_Platform_Width < AsPlatform_Expanding::Max_Expanding_Size)
      {
         Expanding_Platform_Width += Expanding_Platform_Step;
         x_pos -= Expanding_Platform_Step / 2.0;
         // проверка на выходы за пределы рамки
         is_correct = true;
      }
      
      else
         Platform_State->Expanding = EPlatform_Transformation::Active;

      return true;
   
   case EPlatform_Transformation::Finalize:
      if(Expanding_Platform_Width > AsPlatform_Expanding::Min_Expanding_Size)
      {
         Expanding_Platform_Width -= Expanding_Platform_Step;
         x_pos += Expanding_Platform_Step / 2.0;
      }
      else
      {
         Platform_State->Expanding = EPlatform_Transformation::Unknown;
         next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
      }
      return true;
   }

   return false;
}
void AsPlatform_Expanding::Draw_Expanding_State(HDC hdc, double x)
{
   int y = AsConfig::Platform_Y_Pos;
   const double d_scale = AsConfig::D_Global_Scale;
   const int scale = AsConfig::Global_Scale;
   RECT inner_rect = {};
   
   inner_rect.left = (int) ( (x + (Expanding_Platform_Width - (double)Expanding_Platform_Inner_Width) / 2.0) * d_scale);
   inner_rect.top = (y + 1) * scale;
   inner_rect.right = inner_rect.left + Expanding_Platform_Inner_Width * scale;
   inner_rect.bottom = (y + 1 + 5) * scale;

   // 1. Левая сторона
   // 1.1 Шарик
   Draw_Expanding_Ball(hdc, x, true);
   
   // 1.2 Фермы
   Draw_Expanding_Truss(hdc, inner_rect, true);

   // 2 Правая сторона
   // 2.1 Шарик
   Draw_Expanding_Ball(hdc, x, false);
   
   // 2.2 Фермы
   Draw_Expanding_Truss(hdc, inner_rect, false);

   // 3. Средняя часть
   AsCommon::Rect(hdc, inner_rect, *AsPlatform_Expanding::Inner_Color);
}
void AsPlatform_Expanding::Draw_Expanding_Ball(HDC hdc, double x, bool is_left)
{
   RECT rect = {}, arc_rect = {};
   const double d_scale = AsConfig::D_Global_Scale;
   const int scale = AsConfig::Global_Scale;
   
   int arc_mid_x_pos, arc_start_y, arc_end_y, arc_right_offset;
 
   int y = AsConfig::Platform_Y_Pos;

   // Шарик
   if(is_left)
      rect.left = (int)(x * d_scale);
   else
      rect.left = (int) ( (x + Expanding_Platform_Width - (double)AsConfig::Platform_Circle_Size) * d_scale );

   rect.top = y * scale;
   rect.right = rect.left + AsConfig::Platform_Circle_Size * scale;
   rect.bottom = (y + AsConfig::Platform_Circle_Size) * scale;

   AsCommon::Ellipse(hdc, rect, *Circle_Color);

   // 1.2 Переходник на ферму
   if(is_left)
      Rectangle(hdc, rect.left + 4 * scale, rect.top, rect.right - scale + 1, rect.bottom - 1);
   else
      Rectangle(hdc, rect.left + 1, rect.top, rect.left + 3 * scale - 1, rect.bottom - 1);


   // 2. Рисуем блик
   AsConfig::White_Color.Select(hdc);
   Draw_Circle_Highlight(hdc, (int)(x * d_scale), y * scale);

   //1.4 Дуга фермы на шарике

   arc_rect.left = rect.left + 4 * scale + 2;
   arc_rect.top = rect.top + scale + 1;
   arc_rect.right = rect.left + (4 + 3) * scale + 2;
   arc_rect.bottom = rect.bottom - scale - 1;

   arc_mid_x_pos = arc_rect.left + (arc_rect.right - arc_rect.left) / 2;

   if(is_left)
   {
      arc_start_y = arc_rect.top;
      arc_end_y =  arc_rect.bottom;
   }
   else
   {
      arc_start_y = arc_rect.bottom;
      arc_end_y = arc_rect.top;

      arc_right_offset = (AsConfig::Platform_Circle_Size - 2) * scale + 1;

      arc_rect.left -= arc_right_offset;
      arc_rect.right -= arc_right_offset;
      arc_mid_x_pos -= arc_right_offset;
   }
   // 1.5 Дырка в шарике по дугой
   AsCommon::Ellipse(hdc, arc_rect, AsConfig::BG_Color);
  

   // 1.6 Сама дуга
   Truss_Color->Select(hdc);
   Arc(hdc,arc_rect.left, arc_rect.top, arc_rect.right - 1, arc_rect.bottom - 1, arc_mid_x_pos, arc_start_y, arc_mid_x_pos, arc_end_y - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Draw_Expanding_Truss(HDC hdc, RECT &inner_rect, bool is_left)
{ // рисуем фермы расширяющейся плаформы
   int truss_x, truss_top_y, truss_low_y;
   const int scale = AsConfig::Global_Scale;
   double expanding_ratio, truss_x_offset;

   expanding_ratio = (Max_Expanding_Size - Expanding_Platform_Width) / (Max_Expanding_Size - Min_Expanding_Size);
   truss_x_offset =(int)(6.0 * expanding_ratio * AsConfig::D_Global_Scale );

   truss_x = (int)(inner_rect.left + 1);

   if(is_left)
      truss_x += (int)(truss_x_offset + 1.0);
   else
   {
      truss_x += (Expanding_Platform_Inner_Width + 8 - 1) * scale + 1;
      truss_x -= (int)truss_x_offset;
   }

   Truss_Color->Select(hdc);
   
   truss_top_y = inner_rect.top + 1;
   truss_low_y = inner_rect.bottom - scale + 1;

   MoveToEx(hdc, truss_x, truss_top_y, 0);
   LineTo(hdc, truss_x - 4 * scale - 1, truss_low_y);
   LineTo(hdc, truss_x - 8 * scale, truss_top_y);

   MoveToEx(hdc, truss_x, truss_low_y, 0);
   LineTo(hdc, truss_x - 4 * scale - 1, truss_top_y);
   LineTo(hdc, truss_x - 8 * scale, truss_low_y);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Draw_Circle_Highlight(HDC hdc, int x, int y)
{// Рисуем блик на шарике
   const int scale = AsConfig::Global_Scale;
   int circle_size = AsConfig::Platform_Circle_Size - 1;

   Highlight_Color->Select_Pen(hdc);

   Arc(hdc, x + scale, y + scale, x + circle_size * scale - 1, y + circle_size * scale - 1,
      x + 2 * scale, y + scale, x + scale, y + 3 * scale);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Reset()
{
   Expanding_Platform_Width = AsPlatform_Expanding::Min_Expanding_Size;
}
