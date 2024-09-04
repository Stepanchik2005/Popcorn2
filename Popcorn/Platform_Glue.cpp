#include "Platform_Glue.h"


//AsPlatform_Glue
const double AsPlatform_Glue::Max_Glue_Ratio = 1.0;
const double AsPlatform_Glue::Min_Glue_Ratio = 0.2;
const double AsPlatform_Glue::Glue_Spot_Height_Ratio_Step = 0.05;
//------------------------------------------------------------------------------------------------------------
AsPlatform_Glue::AsPlatform_Glue(AsPlatform_State &platform_state)
   : Platform_Glue_Ratio(0.0), Platform_State(&platform_state)
{
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform_Glue::Act(EPlatform_Transformation &glue_state, AsBall_Set *ball_set, EPlatform_State &next_state)
{
   switch(glue_state)
   {
   case EPlatform_Transformation::Init:
      if(Platform_Glue_Ratio < AsPlatform_Glue::Max_Glue_Ratio)
         Platform_Glue_Ratio += Glue_Spot_Height_Ratio_Step;
      else
         glue_state = EPlatform_Transformation::Active;

      return true;

   case EPlatform_Transformation::Finalize:
      if(Platform_Glue_Ratio > AsPlatform_Glue::Min_Glue_Ratio)
      {
         Platform_Glue_Ratio -= Glue_Spot_Height_Ratio_Step;

         while (ball_set->Release_Next_Ball() )
               {
               }
      }
      else
      {	
         glue_state = EPlatform_Transformation::Unknown;
         next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
      }
         
      return true;
   }

    return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Glue::Draw_Glue_State(HDC hdc, double x_pos)
{
   const int scale = AsConfig::Global_Scale;
   const double d_scale = AsConfig::D_Global_Scale;
   HRGN region = {};
   RECT glue_rect = {};
   

   glue_rect.left = (int)( (x_pos + 5.0) * d_scale);
   glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * scale;
   glue_rect.right = glue_rect.left + AsConfig::Platform_Normal_Inner_Width * scale;
   glue_rect.bottom = glue_rect.top + (AsConfig::Platform_Height - 2) * scale;

   region = CreateRectRgnIndirect(&glue_rect);

   SelectClipRgn(hdc, region);

   AsConfig::BG_Color.Select(hdc);

   Draw_Spot_State(hdc, 5, 8, 5 ,x_pos);
   Draw_Spot_State(hdc, 12, 5, 4,x_pos);
   Draw_Spot_State(hdc, 15, 8, 5,x_pos);

   AsConfig::White_Color.Select(hdc);

   Draw_Spot_State(hdc, 6, 6, 4 ,x_pos);
   Draw_Spot_State(hdc, 12, 5, 3,x_pos);
   Draw_Spot_State(hdc, 15, 7, 4,x_pos);

   SelectClipRgn(hdc, 0);
   DeleteObject(region);
}
void AsPlatform_Glue::Draw_Spot_State(HDC hdc, int x_offset, int width, int height, double x_pos)
{// Отрисовка "пятен", которые формируют изображение с клеем
   const int scale = AsConfig::Global_Scale;
   const double d_scale = AsConfig::D_Global_Scale;

   int platform_y = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
   double spot_height = (double)height * Platform_Glue_Ratio;

   RECT rect = {};
   rect.left = (int)( (x_pos + (double)x_offset) * d_scale);
   rect.top = (int)( (double)platform_y - spot_height * d_scale);
   rect.right = rect.left + width * scale;
   rect.bottom = (int)( ( (double)AsConfig::Platform_Y_Pos + spot_height + 1.0) * d_scale);
      
   Chord(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 2, rect.left, platform_y, rect.right, platform_y);
}
void AsPlatform_Glue::Reset()
{
   Platform_Glue_Ratio = AsPlatform_Glue::Min_Glue_Ratio;
}