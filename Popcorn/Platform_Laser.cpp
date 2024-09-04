#include "Platform_Laser.h"


// AsPlatform_Laser
AsPlatform_Laser::~AsPlatform_Laser()
{
   delete Gun_Color;
}
AsPlatform_Laser::AsPlatform_Laser(AsPlatform_State &platform_state, AsLaser_Beam_Set *laser_beam_set)
   : Laser_Transformation_Step(0), Last_Laser_Timer_Shot(0), Enable_Laser_Firing(false), Platform_State(&platform_state),
      Inner_Color(0), Circle_Color(0), Gun_Color(0), Laser_Beam_Set(0)
{
}
void AsPlatform_Laser::Init(AsLaser_Beam_Set *laser_beam_set,AColor &inner_color, AColor &circle_color,  AColor &highlight_color)
{
   Laser_Beam_Set = laser_beam_set;
   Inner_Color = &inner_color;
   Circle_Color = &circle_color;

   Gun_Color = new AColor(highlight_color, AsConfig::Global_Scale);

   //Laser_Beam_Set->Init();
}
bool AsPlatform_Laser::Act_For_Laser_State(EPlatform_State &next_state, double x_pos)
{
   double left_gun_x_pos, right_gun_x_pos;

   switch (Platform_State->Laser)
   {
   case EPlatform_Transformation::Init:
      if (Laser_Transformation_Step < Max_Laser_Transformation_Step)
         ++Laser_Transformation_Step;
      else
         Platform_State->Laser = EPlatform_Transformation::Active;

     /* Redraw_Platform();
      break;*/
      return true;

   case EPlatform_Transformation::Active:
      if(Enable_Laser_Firing)
         if(Last_Laser_Timer_Shot + Laser_Shot_Timeout <= AsConfig::Current_Timer_Tick)
         {
             Last_Laser_Timer_Shot = AsConfig::Current_Timer_Tick + Laser_Shot_Timeout;

             left_gun_x_pos = Get_Gun_Pos(true, x_pos) + 0.3;
             right_gun_x_pos = Get_Gun_Pos(false, x_pos) + 0.3;

             Laser_Beam_Set->Fire(left_gun_x_pos, right_gun_x_pos);
         }
      return false;


   case EPlatform_Transformation::Finalize:
      if (Laser_Transformation_Step > 0)
         --Laser_Transformation_Step;
      else
      {
         Platform_State->Laser = EPlatform_Transformation::Unknown;
         next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
      }

      /*Redraw_Platform();
      break;*/
       return true;

   default:
      AsConfig::Throw();
   }

   return false;
}
void AsPlatform_Laser::Draw_Laser_State(HDC hdc, double x, RECT &platform_rect)
{// Рисуем лазерную платформу

   HRGN region;
   

   region = CreateRectRgnIndirect(&platform_rect);
   SelectClipRgn(hdc, region);

   // 1. Левое крыло
   Draw_Laser_Wing(hdc, x, true);

   // 2. Правое крыло
   Draw_Laser_Wing(hdc, x, false);


   // 3. Центральная часть
   Draw_Inner_Part(hdc, x);

   // 3.1. Левая 
   Draw_Laser_Leg(hdc, x, true);

   // 3.2. Правая нога
   Draw_Laser_Leg(hdc, x, false);


   // 3.3. Кабина
   Draw_Laser_Cabin(hdc, x);

   SelectClipRgn(hdc, 0);
   DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Reset()
{
    Laser_Transformation_Step = 0;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Fire(bool is_enable_firing, double x)
{
   if(Platform_State->Laser != EPlatform_Transformation::Active)
      return;

   Enable_Laser_Firing = is_enable_firing;
}
void AsPlatform_Laser::Draw_Inner_Part(HDC hdc, double x)
{
   double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
   double y;
   // Нормальная средняя часть
   // Размер 20 х 5 --> 0 x 0 
   //x = X_Pos;
   y = AsConfig::Platform_Y_Pos;

   Inner_Color->Select(hdc);
   Draw_Expanding_Figure(hdc, EFigure_Type::Round_Rect, x + 4, y + 1, 20, 5, ratio, x + 10, y + 3, 8, 1);
}
void AsPlatform_Laser::Draw_Laser_Wing(HDC hdc, double x_pos, bool is_left)
{// Рисуем крыло лазерной платформы

   double x, y;
   int x_offset;
   int half_max_lazer_transformation_step = Max_Laser_Transformation_Step / 2;
   double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;
   double height;
   const int scale = AsConfig::Global_Scale;
   const double d_scale = AsConfig::D_Global_Scale;
   //AsConfig::BG_Color.Select(hdc);
   Circle_Color->Select(hdc);

   x = x_pos;

   if(! is_left)
      x += (AsConfig::Platform_Normal_Width - AsConfig::Platform_Circle_Size);

   y = AsConfig::Platform_Y_Pos;
   // 1. Само крыло
   Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x, y, 7, 7, ratio, x, y, 7, 12);
   
   // 1.1. Перемычка
   // (3 : 6) -> (5 : 2)
   // размер 6х5
   if(is_left)
      x_offset = 5;
   else
      x_offset = -4;

   Draw_Expanding_Figure(hdc, EFigure_Type::Rectangle, x + 3, y + 6, 1, 1, ratio, x + x_offset, y + 2, 6, 5);

   
   // 1.2. Пушка
   if(Laser_Transformation_Step >= half_max_lazer_transformation_step)
   {

      ratio = ((double)Laser_Transformation_Step - half_max_lazer_transformation_step) / half_max_lazer_transformation_step;

      Gun_Color->Select(hdc);

     /* if (is_left)
         x = (x_pos + 3.0);
      else
         x = x_pos + (AsPlatform::Normal_Width - 4) ;*/

      x = Get_Gun_Pos(is_left, x_pos);

      height = 3.0 * (1.0 - ratio) * AsConfig::D_Global_Scale;

      MoveToEx(hdc, (int)(x * d_scale + 1.0), (int)(y * d_scale + 3.0 * d_scale + 1.0), 0);
      LineTo(hdc, (int)(x * d_scale + 1.0), (int)(y * d_scale + height + 1.0) );

      // 1.3. Хвост
      Draw_Expanding_Figure(hdc, EFigure_Type::Rectangle, x + 1, y + 5, 0, 0, ratio, x - 1, y + 5 + 1.0 / AsConfig::Global_Scale, 3, 6);
   }
   
}

void AsPlatform_Laser::Draw_Laser_Leg(HDC hdc, double x_pos, bool is_left)
{// Рисуем "ногу" лазерной платформы

   double x, y;
   double x_scale;
   double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

   //const int scale = AsConfig::Global_Scale;
   const double d_scale = AsConfig::D_Global_Scale;

   Inner_Color->Select(hdc);

   if (is_left)
   {
      x = (x_pos + 6.0) * d_scale;
      x_scale = d_scale;
   }
   else
   {
      x = x_pos * d_scale + (AsConfig::Platform_Normal_Width - 6) * d_scale - 1.0;
      x_scale = -d_scale;
   }

   y = (AsConfig::Platform_Y_Pos + 3) * d_scale;

   POINT left_leg_points[7] =
   {
      { (int)x, (int)y }, { (int)(x + 2.0 * x_scale), (int)(y - 2.0 * d_scale) }, { (int)(x + 4.0 * x_scale),(int)(y - 2.0 * d_scale) }, {(int)(x + 4.0 * x_scale), (int)y },
      { (int)(x + 2.0 * x_scale), (int)(y + 2.0 * d_scale) }, { (int)(x + 2.0 * x_scale), (int)(y + (2.0 + 2.0 * ratio) * d_scale)  }, { (int)x, (int)(y + 4.0 * ratio * d_scale) } 
   };

   Polygon(hdc, left_leg_points, 7);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Laser_Cabin(HDC hdc, double x_pos)
{// Рисуем кабину лазерной платформы

   double x, y;
   const int scale = AsConfig::Global_Scale;
   double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

   x = x_pos;
   y = AsConfig::Platform_Y_Pos;

   // 1. Внешняя часть
   //  Размер: 2 х 1 -->  10 х 8
  Inner_Color->Select(hdc);

   Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x + 13, y + 1, 2, 1, ratio, x + 9, y - 1, 10, 8 - 1.0 / AsConfig::D_Global_Scale );
   // 2. Среднее кольцо
   //  Размер: 2 х 1 -->  8 х 6
   
   AsConfig::BG_Color.Select(hdc);
   
   Draw_Expanding_Figure(hdc, EFigure_Type::Rectangle, x + 13, y + 1, 2, 1, ratio, x + 11, y, 6, 1);
   Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x + 13, y + 1, 2, 1, ratio, x + 10, y, 8, 5 - 1.0 / AsConfig::D_Global_Scale);

   // 3. Внутренняя часть
   // Размер: 2 х 1 -->  6 х 4
   
   AsConfig::White_Color.Select(hdc);

   Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x + 13, y + 1, 2, 1, ratio, x + 11, y, 6, 4 - 1.0 / AsConfig::D_Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Expanding_Figure(HDC hdc, EFigure_Type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height)
{
   double x, y;
   double width, height;
   RECT rect = {};
   x = Get_Expanding_Value(hdc, start_x, end_x, ratio);
   y = Get_Expanding_Value(hdc, start_y, end_y, ratio);
   width = Get_Expanding_Value(hdc, start_width, end_width, ratio);
   height = Get_Expanding_Value(hdc, start_height, end_height, ratio);

   switch(figure_type)
   {
      case EFigure_Type::Ellipse:
         Ellipse(hdc, (int)x, (int)y, (int)(x + width - 1.0), (int)(y + height - 1.0));
         break;
      case EFigure_Type::Rectangle:
         Rectangle(hdc, (int)x, (int)y, (int)(x + width - 1.0), (int)(y + height - 1.0));
         break;
      case EFigure_Type::Round_Rect:
         rect.left = (int)x;
         rect.top = (int)y;
         rect.right = (int)(x + width);
         rect.bottom = (int)(y + height);

         AsCommon::Round_Rect(hdc, rect, 3);
         break;

      default:
         AsConfig::Throw();
   }		
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform_Laser::Get_Expanding_Value(HDC hdc, double start_pos, double end_pos, double ratio)
{
   double delta;
   double value;
   delta = end_pos - start_pos; 

   value =  (start_pos + delta * ratio) * AsConfig::D_Global_Scale;

   return value;
}
//------------------------------------------------------------------------------------------------------------

double AsPlatform_Laser::Get_Gun_Pos(bool is_left, double platform_x_pos)
{
   double gun_pos;
    if (is_left)
         gun_pos = (platform_x_pos + 3.0);
      else
         gun_pos = platform_x_pos + (AsConfig::Platform_Normal_Width - 4) ;

    return gun_pos;
}

