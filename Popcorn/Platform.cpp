#include "Platform.h"
//AsPlatform_State
AsPlatform_State::AsPlatform_State()
   : Regular(EPlatform_Substate_Regular::Missing), Meltdown(EPlatform_Substate_Meltdown::Unknown),Rolling(EPlatform_Substate_Rolling::Unknown), 
     Glue(EPlatform_Transformation::Unknown), Expanding(EPlatform_Transformation::Unknown),Laser(EPlatform_Transformation::Unknown), Moving(EPlatform_Moving_State::Stop),
     Current_State(EPlatform_State::Regular), Next_State(EPlatform_State::Unknown)
{
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::Set_Next_State(EPlatform_State next_state)
{
   
   if(next_state == Current_State)
      return;

   switch(Current_State)
   {
      case EPlatform_State::Regular:
         AsConfig::Throw(); // Обычное состояние "само" не заканчивается, переключение  в другое состояние должно быть явным
         break;
      case EPlatform_State::Meltdown:
         return; // Игнорируем любое переключение состояние т.к. после Meltdown мы должны рестартнуть уровень

      case EPlatform_State::Rolling:
         AsConfig::Throw(); // это состояние должно "само" переключаться в следующее
         break;
      case EPlatform_State::Glue:
         Glue = EPlatform_Transformation::Finalize;
         break;
      case EPlatform_State::Expanding:
         Expanding = EPlatform_Transformation::Finalize;
         break;
      case EPlatform_State::Laser:
         Laser = EPlatform_Transformation::Finalize;
         break;
   }
   Next_State = next_state;
}

EPlatform_State AsPlatform_State::Get_Next_State()
{
   return Next_State;
}
EPlatform_State AsPlatform_State::Set_State(EPlatform_Substate_Regular new_regular_state)
{
   EPlatform_Transformation *transformation_state = 0;

 
   if(Current_State == EPlatform_State::Regular && Regular == new_regular_state)
      return EPlatform_State::Unknown;

   if(new_regular_state == EPlatform_Substate_Regular::Normal)
   {
      switch(Current_State)
      {
         case EPlatform_State::Glue:
         {
            transformation_state = &Glue;
            break;
         }
         case EPlatform_State::Expanding:
         {
             transformation_state = &Expanding;
            break;
         }
         case EPlatform_State::Laser:
         {
            transformation_state = &Laser;
            break;
         }
      }
   }
   if(transformation_state != 0)
   {
      if(*transformation_state == EPlatform_Transformation::Unknown)
      {//Финализация закончилась		
         return Set_New_Or_Regular_State(new_regular_state);

         //if(next_state == EPlatform_State::Unknown) // если нету отложенного состояния, то выходим, иначе - переводим
         //   return;
            
     

          /*Set_State(next_state);     */
      }
      else
      {// Финализация начинается	
         *transformation_state = EPlatform_Transformation::Finalize;
         return EPlatform_State::Unknown; // EPS_Normal устанавливаем в методе Act() только после проигрывания обратной анимации растекающегося клея
      }
   }
   

   Current_State = EPlatform_State::Regular;
   Regular = new_regular_state;

   return EPlatform_State::Unknown;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform_State::Set_New_Or_Regular_State(EPlatform_Substate_Regular new_state)
{
   EPlatform_State next_state;

   Current_State = EPlatform_State::Regular;

   
   next_state = Get_Next_State();

   //if(next_state != EPlatform_State::Unknown)// Если есть отложенное состояние, то переводим в него
   //  
   //   //Set_State(next_state);
   //else
   if(next_state == EPlatform_State::Unknown)
      Regular = new_state;		

    return next_state;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform_State::operator EPlatform_State() const
{
   return Current_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_State::operator = (EPlatform_State new_state)
{
   Current_State = new_state;
}




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
  
   HRGN region;
   RECT glue_rect;

   glue_rect.left = (x_pos + 5) * AsConfig::Global_Scale;
   glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
   glue_rect.right = glue_rect.left + AsPlatform::Normal_Platform_Inner_Width * AsConfig::Global_Scale;
   glue_rect.bottom = glue_rect.top + (AsPlatform::Height - 2) * AsConfig::Global_Scale;

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
   RECT rect;
   int platform_y = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
   double spot_height = (double)height * Platform_Glue_Ratio;

   rect.left = (x_pos + x_offset) * AsConfig::Global_Scale;
   rect.top = platform_y - spot_height * AsConfig::Global_Scale;
   rect.right = rect.left + width * AsConfig::Global_Scale;
   rect.bottom = (AsConfig::Platform_Y_Pos + spot_height + 1) * AsConfig::Global_Scale;
      
   Chord(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 2, rect.left, platform_y, rect.right, platform_y);
}
void AsPlatform_Glue::Reset()
{
   Platform_Glue_Ratio = AsPlatform_Glue::Min_Glue_Ratio;
}




// AsPlatform_Expanding
const double AsPlatform_Expanding::Max_Expanding_Size = 40.0; 
const double AsPlatform_Expanding::Min_Expanding_Size = (double)AsPlatform::Normal_Width;
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
        /* Correct_Platform_Pos();*/
      }
      
      else
         Platform_State->Expanding = EPlatform_Transformation::Active;

     /* Redraw_Platform();
      break;*/
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

     /* Redraw_Platform();
      break;*/
      return true;
   }

   return false;
}
void AsPlatform_Expanding::Draw_Expanding_State(HDC hdc, double x)
{
   //double x = X_Pos;
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
   AsPlatform_Expanding::Inner_Color->Select(hdc);

   Rectangle(hdc, inner_rect.left, inner_rect.top, inner_rect.right - 1, inner_rect.bottom - 1);
}
void AsPlatform_Expanding::Draw_Expanding_Ball(HDC hdc, double x, bool is_left)
{
   RECT rect, arc_rect;
   const double d_scale = AsConfig::D_Global_Scale;
   const int scale = AsConfig::Global_Scale;
   
   int arc_mid_x_pos, arc_start_y, arc_end_y, arc_right_offset;
   //double x = X_Pos;

   int y = AsConfig::Platform_Y_Pos;

   // Шарик
   if(is_left)
      rect.left = (int)(x * d_scale);
   else
      rect.left = (int) ( (x + Expanding_Platform_Width - (double)AsPlatform::Circle_Size) * d_scale );

   rect.top = y * scale;
   rect.right = rect.left + AsPlatform::Circle_Size * scale;
   rect.bottom = (y + AsPlatform::Circle_Size) * scale;

   Circle_Color->Select(hdc);
   Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);

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

      arc_right_offset = (AsPlatform::Circle_Size - 2) * scale + 1;

      arc_rect.left -= arc_right_offset;
      arc_rect.right -= arc_right_offset;
      arc_mid_x_pos -= arc_right_offset;
   }
   // 1.5 Дырка в шарике по дугой
   AsConfig::BG_Color.Select(hdc);
   Ellipse(hdc,arc_rect.left,arc_rect.top,arc_rect.right - 1, arc_rect.bottom - 1);

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
   int circle_size = AsPlatform::Circle_Size - 1;

   Highlight_Color->Select_Pen(hdc);

   Arc(hdc, x + scale, y + scale, x + circle_size * scale - 1, y + circle_size * scale - 1,
      x + 2 * scale, y + scale, x + scale, y + 3 * scale);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Reset()
{
   Expanding_Platform_Width = AsPlatform_Expanding::Min_Expanding_Size;
}




// AsPlatform
AsPlatform_Laser::~AsPlatform_Laser()
{
   delete Gun_Color;
}
AsPlatform_Laser::AsPlatform_Laser(AsPlatform_State &platform_state)
   : Laser_Transformation_Step(0), Platform_State(&platform_state), Inner_Color(0), Circle_Color(0), Gun_Color(0)
{
}
void AsPlatform_Laser::Init(AColor &inner_color, AColor &circle_color,  AColor &highlight_color)
{
   Inner_Color = &inner_color;
   Circle_Color = &circle_color;

   Gun_Color = new AColor(highlight_color, AsConfig::Global_Scale);
}
bool AsPlatform_Laser::Act_For_Laser_State(EPlatform_State &next_state)
{
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
      x += (AsPlatform::Normal_Width - AsPlatform::Circle_Size);

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

      if (is_left)
         x = (x_pos + 3.0);
      else
         x = x_pos + (AsPlatform::Normal_Width - 4) ;

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
      x = x_pos * d_scale + (AsPlatform::Normal_Width - 6) * d_scale - 1.0;
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
   RECT rect;
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

         AsConfig::Round_Rect(hdc, rect, 3);
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
void AsPlatform_Laser::Reset()
{
    Laser_Transformation_Step = 0;
}




// AsPlatform

//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
: X_Pos(AsConfig::Border_X_Offset), Platform_State(), Platform_Glue(Platform_State), Platform_Expanding(Platform_State), Platform_Laser(Platform_State) ,Inner_Width(Normal_Platform_Inner_Width), 
  Rolling_Step(0), Normal_Platform_Image_Width(0), // !!!!!
  Normal_Platform_Image_Height(0), Last_Timer_Tick(0),Left_Key_Dowm(false), Right_Key_Down(false), Speed(0.0), Normal_Platform_Image(0),Platform_Rect{}, 
  Prev_Platform_Rect{}, Highlight_Color(255, 255, 255), Platform_Circle_Color(151, 0, 0), Platform_Inner_Color(0, 128, 192)

{
   X_Pos = (AsConfig::Max_X_Pos - Normal_Width) / 2;
}

//------------------------------------------------------------------------------------------------------------
AsPlatform::~AsPlatform()	
{
   delete [] Normal_Platform_Image;
}

//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{
   
   double inner_top_y; 
   double inner_low_y;
   double inner_left_x, inner_right_x;
   double reflection_pos;
   double inner_y;
   double ball_x, ball_y;
   int width;
   if(next_y_pos + ball->Radius < AsConfig::Platform_Y_Pos)
      return false;
   
   width = Get_Platform_Width();

   inner_top_y = (double) (AsConfig::Platform_Y_Pos + 1);
   inner_left_x = (double) (X_Pos + (Circle_Size - 1));
   inner_right_x = (double) (X_Pos + width - (Circle_Size - 1));
   inner_low_y = (double) (AsConfig::Platform_Y_Pos + (Height - 1));

   //1. Проверка отражения от мячика
   if(Reflect_On_Circle( next_x_pos, next_y_pos, 0, ball))
       goto _on_hit;// от левого
   
   if(Reflect_On_Circle( next_x_pos, next_y_pos, width - Circle_Size, ball))
       goto _on_hit; // от правого


   if(ball->Is_Moving_Up() )
      inner_y = inner_low_y;
   else
      inner_y = inner_top_y;
   
   
   if (Hit_Circle_On_Line(next_y_pos - inner_y, next_x_pos, inner_left_x, inner_right_x, ball->Radius, reflection_pos) ) // отрадежение от внутренней части платформы
   {
      ball->Reflect(true);
      goto _on_hit;
   }

   return false;

_on_hit:
   if(ball->Get_State() == EBS_On_Paraschute)
      ball->Set_State(EBS_Off_Paraschute);

   if(Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active)
   {
      ball->Get_Center(ball_x, ball_y);
      ball->Set_State(EBS_On_Platform, ball_x, ball_y);
   }
   return true;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act()
{
   EPlatform_State next_state = EPlatform_State::Unknown;
   bool is_correct;
   switch (Platform_State)
   {
      case EPlatform_State::Rolling:
         Act_For_Rolling_State();
         break;
      
      case EPlatform_State::Glue:
         if(Platform_Glue.Act(Platform_State.Glue, Ball_Set, next_state) )
             Redraw_Platform();
      
         if(next_state != EPlatform_State::Unknown)
            Set_State(next_state);

         break;

      case EPlatform_State::Meltdown:
         Act_For_Meltdown_State();
         break;

      case EPlatform_State::Expanding:
         if(Platform_Expanding.Act_For_Expanding_State(X_Pos, is_correct, next_state) )
             Redraw_Platform();

         if(is_correct)
            Correct_Platform_Pos();

         if(next_state != EPlatform_State::Unknown)
            Set_State(next_state);

         break;
      case EPlatform_State::Laser:
         if(Platform_Laser.Act_For_Laser_State(next_state) )
            Redraw_Platform();

         if(next_state != EPlatform_State::Unknown)
            Set_State(next_state); // устанока нового состояния 

         break;
   }
}
bool AsPlatform::Is_Finished()
{
   return false;
}
void AsPlatform::Draw(HDC hdc, RECT &paint_area)
{
   // Рисуем платформу

   RECT intersection_rect;

   if (! IntersectRect(&intersection_rect, &paint_area, &Platform_Rect) )
      return;

   switch (Platform_State)
   {
   case EPlatform_State::Regular:
      if(Has_Substate_Regular( EPlatform_Substate_Regular::Ready) || Has_Substate_Regular(EPlatform_Substate_Regular::Normal) )
         Draw_Normal_State(hdc, paint_area);
      break;

   case EPlatform_State::Meltdown:
      if(Platform_State.Meltdown == EPlatform_Substate_Meltdown::Active)
         Draw_Meltdown_State(hdc, paint_area);
      break;
   
   case EPlatform_State::Rolling:
      switch(Platform_State.Rolling)
      {
      
      case EPlatform_Substate_Rolling::Roll_In:
         Draw_Roll_In_State(hdc, paint_area);
         break;

      case EPlatform_Substate_Rolling::Expand_Roll_In:
         Draw_Normal_State(hdc, paint_area); // отрисовка нормального состояния, но с постоянным смещением по оси Х в методе Act()
         break;
      }
      break;

   case EPlatform_State::Glue:
      Draw_Normal_State(hdc, paint_area);
      Platform_Glue.Draw_Glue_State(hdc, X_Pos);
      break;

   case EPlatform_State::Expanding:
      Platform_Expanding.Draw_Expanding_State(hdc, X_Pos);
      break;

   case EPlatform_State::Laser:
      Platform_Laser.Draw_Laser_State(hdc, X_Pos, Platform_Rect);
      break;
   }
}
void AsPlatform::Clear(HDC hdc, RECT &paint_area)
{
   switch (Platform_State)
   {
   case EPlatform_State::Regular:
      if(! ( Has_Substate_Regular(EPlatform_Substate_Regular::Ready) || Has_Substate_Regular(EPlatform_Substate_Regular::Normal) ))
         break;
      // else - no break
   case EPlatform_State::Rolling:
   case EPlatform_State::Glue:
   case EPlatform_State::Expanding:
   case EPlatform_State::Laser:
      Clear_BG(hdc, paint_area);
      break;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set *ball_set)
{
   Ball_Set = ball_set;

   Platform_Expanding.Init(Platform_Inner_Color, Platform_Circle_Color, Highlight_Color);
   Platform_Laser.Init(Platform_Inner_Color, Platform_Circle_Color, Highlight_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::On_Space_Key(bool key_down)
{
   if(! key_down)
      return;
   
   if(Has_Substate_Regular(EPlatform_Substate_Regular::Ready) )
   {
      Ball_Set->Release_From_Platform(Get_Middle_Pos());
      Platform_State.Set_State(EPlatform_Substate_Regular::Normal);
   }
   else 
      if(Platform_State == EPlatform_State::Glue)
         Ball_Set->Release_Next_Ball();
   else if (Platform_State == EPlatform_State::Laser)
         AsConfig::Throw();  //!!! Надо сделать!
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed)
{
   double step;

   if(Platform_State.Moving == EPlatform_Moving_State::Stopping || Platform_State.Moving == EPlatform_Moving_State::Stop)
      return;

   step = Speed / max_speed * AsConfig::Moving_Size_Step;
   X_Pos += step;

   if(Correct_Platform_Pos())
   {
      Speed = 0.0;	
      Platform_State.Moving = EPlatform_Moving_State::Stopping;
   }
   

   // cмещаем приклеенные мячики
   if( (Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active) || Has_Substate_Regular(EPlatform_Substate_Regular::Ready) )
   {
      if(Platform_State.Moving == EPlatform_Moving_State::Left)
         Ball_Set->On_Platform_Advance(M_PI, fabs(Speed), max_speed);
      else
         if(Platform_State.Moving == EPlatform_Moving_State::Right)
            Ball_Set->On_Platform_Advance(0, fabs(Speed), max_speed);
   }
}

//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Speed()
{
   return Speed;
}
//------------------------------------------------------------------------------------------------------------

void AsPlatform::Start_Movement()
{
   Prev_Platform_Rect = Platform_Rect;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::End_Movement()
{
   if(Platform_State.Moving == EPlatform_Moving_State::Stop)
      return;

   Redraw_Platform();

   if(Platform_State.Moving == EPlatform_Moving_State::Stopping)
      Platform_State.Moving = EPlatform_Moving_State::Stop;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform::Get_State()
{
   return Platform_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_State new_state)
{
   int i, len;

   if (Platform_State == new_state)
      return;

   switch (new_state)
   {
   case EPlatform_State::Regular:
      AsConfig::Throw(); // это состояние будем устанавливать в другом Set_State()

   case EPlatform_State::Meltdown:
      if(Platform_State != EPlatform_State::Regular)
      {
         Platform_State.Set_Next_State(new_state);
         return;
      }
      if(Platform_State.Meltdown == EPlatform_Substate_Meltdown::Unknown)
         Platform_State.Meltdown = EPlatform_Substate_Meltdown::Init;

      Speed = 0.0;
      len = sizeof(Meltdown_Platform_Y_Pos) / sizeof(Meltdown_Platform_Y_Pos[0]);

      for (i = 0; i < len; i++)
         Meltdown_Platform_Y_Pos[i] = Platform_Rect.top;

      break; 

   case EPlatform_State::Rolling:
      if(Platform_State.Rolling != EPlatform_Substate_Rolling::Unknown)
         return;

      Platform_State.Rolling = EPlatform_Substate_Rolling::Roll_In;

      X_Pos = AsConfig::Max_X_Pos - 1;
      Rolling_Step = Max_Rolling_Step - 1;
      break;


   case EPlatform_State::Glue:
      if(! Set_Transformation_State(new_state, Platform_State.Glue))
         return;

      Platform_Glue.Reset();

      break;
      
   case EPlatform_State::Expanding:
      if(! Set_Transformation_State(new_state, Platform_State.Expanding))
         return;

     Platform_Expanding.Reset();
      
      break;

   case EPlatform_State::Laser:
      if(! Set_Transformation_State(new_state, Platform_State.Laser))
         return;

     Platform_Laser.Reset();
      
      break;
   }
   
   Platform_State = new_state;
}
void AsPlatform::Set_State(EPlatform_Substate_Regular new_regular_state)
{
   EPlatform_State next_state;   

   next_state = Platform_State.Set_State(new_regular_state);

   if(next_state != EPlatform_State::Unknown)
      Set_State(next_state);
}

bool AsPlatform::Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state)
{
   if(Platform_State != EPlatform_State::Regular)
   {
      Platform_State.Set_Next_State(new_state);
      return false;
   }

   if(transformation_state == EPlatform_Transformation::Finalize)
      return false;

   else
   {
      transformation_state = EPlatform_Transformation::Init;
      return true;
   }
}


//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Has_Substate_Regular(EPlatform_Substate_Regular new_state)
{
   if(Platform_State.Regular == new_state)
      return true;
   else 
      return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw_Platform()
{
   double platform_width;

   if(Last_Timer_Tick != AsConfig::Current_Timer_Tick)
   {
      Prev_Platform_Rect = Platform_Rect;
      Last_Timer_Tick = AsConfig::Current_Timer_Tick;
   }

      

   platform_width = Get_Platform_Width();

   Platform_Rect.left = (int) ( X_Pos * AsConfig::D_Global_Scale);
   Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
   Platform_Rect.right = Platform_Rect.left +(int) ( platform_width * AsConfig::D_Global_Scale);
   Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

   if (Platform_State == EPlatform_State::Meltdown)
      Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;
   

   AsConfig::Invalidate_Rect(Prev_Platform_Rect);
   AsConfig::Invalidate_Rect(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------

 void AsPlatform::Move(bool is_left, bool key_down)
 {
    if(! (Has_Substate_Regular(EPlatform_Substate_Regular::Normal) || Platform_State == EPlatform_State::Glue || Platform_State == EPlatform_State::Laser))
       return;
      
    if(is_left)
       Left_Key_Dowm = key_down;
    else
       Right_Key_Down = key_down;

    if(Left_Key_Dowm && Right_Key_Down)
       return;
    
    if(! Left_Key_Dowm && ! Right_Key_Down)
    {
       Speed = 0.0;
       Platform_State.Moving = EPlatform_Moving_State::Stopping;
       return;
    }

    if(Left_Key_Dowm)
    {
       Platform_State.Moving = EPlatform_Moving_State::Left;
       Speed = -X_Step;
    }

    if(Right_Key_Down)
    {
      Platform_State.Moving = EPlatform_Moving_State::Right;
      Speed = X_Step;
    }
 }
 
 //------------------------------------------------------------------------------------------------------------
 bool AsPlatform::HitBy(AFalling_Letter *falling_letter)
 {
   RECT intersection_rect, falling_letter_rect;

   falling_letter->Get_Letter_Cell(falling_letter_rect);

   if (IntersectRect(&intersection_rect, &falling_letter_rect, &Platform_Rect))
      return true;
   else
      return false;
 }
 //------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Middle_Pos()
{
   return (double) (X_Pos + (double)Normal_Width / 2.0);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Rolling_State()
{
   switch(Platform_State.Rolling)
   {
      case EPlatform_Substate_Rolling::Roll_In:
         ++Rolling_Step;

         if (Rolling_Step >= Max_Rolling_Step)
            Rolling_Step -= Max_Rolling_Step;

         X_Pos -= Rolling_Platform_Speed;

         if (X_Pos <= Roll_In_Platform_End_X_Pos)
         {
            X_Pos = Roll_In_Platform_End_X_Pos;
            Platform_State.Rolling = EPlatform_Substate_Rolling::Expand_Roll_In;
            Inner_Width = 1;
         }
         break;

      case EPlatform_Substate_Rolling::Expand_Roll_In:
         --X_Pos;
         Inner_Width += 2;

         if (Inner_Width >= Normal_Platform_Inner_Width)
         {
            Inner_Width = Normal_Platform_Inner_Width;
            Set_State(EPlatform_Substate_Regular::Ready);
            Platform_State.Rolling = EPlatform_Substate_Rolling::Unknown;
      
         }
         break;
   }

   Redraw_Platform();

}

void AsPlatform::Act_For_Meltdown_State()
{
   switch(Platform_State.Meltdown)
   {
      case EPlatform_Substate_Meltdown::Init:
         Platform_State.Meltdown = EPlatform_Substate_Meltdown::Active;
         break;

      case EPlatform_Substate_Meltdown::Active:
         Redraw_Platform();
         break;
   }
}


bool AsPlatform::Correct_Platform_Pos()
{// проверка на выходы за пределы рамки и корректировка, если таковая потребуется
   double max_platform_x;

   if(X_Pos <= AsConfig::Border_X_Offset)
   {
      X_Pos = AsConfig::Border_X_Offset;
      return true;
   }

   max_platform_x = AsConfig::Max_X_Pos - Get_Platform_Width() + 1;

   if(X_Pos >= max_platform_x)
   {
      X_Pos = max_platform_x;
      return true;
   }

   return false;
}
void AsPlatform::Clear_BG(HDC hdc, RECT &paint_area)
{// Очищаем фоном прежнее место
   RECT intersection_rect;

   if ( !(IntersectRect(&intersection_rect, &paint_area, &Prev_Platform_Rect)) )
      return;

   AsConfig::BG_Color.Select(hdc);
   Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
}
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Normal_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в нормальном состоянии
   
   double x = X_Pos;
   int y = AsConfig::Platform_Y_Pos;
   const double d_scale = AsConfig::D_Global_Scale;
   const int scale = AsConfig::Global_Scale;
   RECT inner_rect = {};
   //Clear_BG(hdc);

   // 1. Рисуем боковые шарики
   AsPlatform::Platform_Circle_Color.Select(hdc);

   Ellipse(hdc, (int)(x * d_scale), y * scale, (int)((x + (double)Circle_Size) * d_scale - 1), (y + Circle_Size) * scale - 1);
   Ellipse(hdc,(int)( (x + (double)Inner_Width) * d_scale), y * scale, (int)((x + (double)Circle_Size + (double)Inner_Width) * d_scale - 1), (y + Circle_Size) * scale - 1);

   // 2. Рисуем блик
   Platform_Expanding.Draw_Circle_Highlight(hdc, x * d_scale, y * scale);

   // 3. Рисуем среднюю часть
   AsPlatform::Platform_Inner_Color.Select(hdc);

   inner_rect.left = (x + 4) * d_scale;
   inner_rect.top = (y + 1) * scale;
   inner_rect.right = (x + 4 + Inner_Width - 1) * d_scale;
   inner_rect.bottom = (y + 1 + 5) * scale;

   AsConfig::Round_Rect(hdc, inner_rect, 3);

   
   if(Normal_Platform_Image == 0 && Has_Substate_Regular(EPlatform_Substate_Regular::Ready) )
      Get_Normal_Platform_Image(hdc);
   
}


//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
   int i,j;
   int x, y;
   int offset = 0;

   x = (int) (X_Pos * AsConfig::D_Global_Scale);
   y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;

   Normal_Platform_Image_Width = Normal_Width * AsConfig::Global_Scale;
   Normal_Platform_Image_Height = Height * AsConfig::Global_Scale;

   Normal_Platform_Image = new int[Normal_Platform_Image_Width * Normal_Platform_Image_Height];

   for(i = 0; i < Normal_Platform_Image_Height; ++i)
      for(j = 0; j < Normal_Platform_Image_Width; ++j)
         Normal_Platform_Image[offset++] = GetPixel(hdc, x + j, y + i );
   
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в расплавленном состоянии

   int i, j;
   int x, y;
   int y_offset;
   int stroke_length = 0;
   int moved_columns_count = 0;
   int max_platform_y;
   AColor color;

   max_platform_y = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

   for (i = 0; i < Normal_Platform_Image_Width; i++)
   {
      if (Meltdown_Platform_Y_Pos[i] > max_platform_y)
         continue;

      ++moved_columns_count;

      y_offset = AsConfig::Rand(Meltdown_Speed) + 1;
      x = Platform_Rect.left + i;

      j = 0;
      y = Meltdown_Platform_Y_Pos[i];

      MoveToEx(hdc, x, y, 0);

      while(Get_Platform_Image_Stroke_Color(i, j, color, stroke_length))
      {
         color.Select_Pen(hdc);

         LineTo(hdc, x, y + stroke_length);

         y += stroke_length;
         j += stroke_length;
      }

      y = Meltdown_Platform_Y_Pos[i];
      MoveToEx(hdc, x, y, 0);
      AsConfig::BG_Color.Select_Pen(hdc);

      LineTo(hdc, x, y + y_offset);

      Meltdown_Platform_Y_Pos[i] += y_offset;
   }




   if (moved_columns_count == 0)
   {
      Platform_State.Set_State(EPlatform_Substate_Regular::Missing);  // Вся платформа сдвинулась за пределы окна
      Platform_State.Meltdown = EPlatform_Substate_Meltdown::Unknown;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Roll_In_State(HDC hdc, RECT &paint_area)
{// Рисуем выкатывающуюся платформу

   int x = (int) (X_Pos * AsConfig::D_Global_Scale);
   int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
   int roller_size = Circle_Size * AsConfig::Global_Scale;
   double alpha;
   XFORM xform = {}, old_xform = {};

   //Clear_BG(hdc);

   // 1. Шарик
   AsPlatform::Platform_Circle_Color.Select(hdc);
   Ellipse(hdc, x, y, x + roller_size - 1, y + roller_size - 1);


   // 2. Разделительная линия

   alpha = -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;

   xform.eM11 = (float)cos(alpha);
   xform.eM12 = (float)sin(alpha);
   xform.eM21 = (float)-sin(alpha);
   xform.eM22 = (float)cos(alpha);
   xform.eDx = (float)(x + roller_size / 2);
   xform.eDy = (float)(y + roller_size / 2);
   GetWorldTransform(hdc, &old_xform);
   SetWorldTransform(hdc, &xform);

   AsConfig::BG_Color.Select(hdc);

   Rectangle(hdc, -AsConfig::Global_Scale / 2, -roller_size / 2, AsConfig::Global_Scale / 2, roller_size / 2);

   SetWorldTransform(hdc, &old_xform);


   // 3. Блик
   Platform_Expanding.Draw_Circle_Highlight(hdc, x, y);

   
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball)
{
   double dx, dy;
   double distance;
   double two_radiuses;
   double platform_ball_radius,platform_ball_x,platform_ball_y;
   double beta, alpha, gamma;
   double related_ball_direction;
   double const pi_2 = 2.0 * M_PI;

   platform_ball_radius = (double)Circle_Size / 2.0;
   platform_ball_x =  X_Pos + platform_ball_radius + platform_ball_x_offset;
   platform_ball_y = (double)(AsConfig::Platform_Y_Pos + platform_ball_radius);
   
   dx = next_x_pos - platform_ball_x;
   dy = next_y_pos - platform_ball_y;

   distance = sqrt(dx * dx + dy * dy);
   two_radiuses = ball->Radius + platform_ball_radius;

   if(fabs(distance - two_radiuses < AsConfig::Moving_Size_Step))
   {// Мячик коснулся шарика
      beta = atan2(-dy, dx);

      related_ball_direction = ball->Get_Direction();
      related_ball_direction -= beta;

      if(related_ball_direction > pi_2)
         related_ball_direction -= pi_2;

      if(related_ball_direction < 0)
         related_ball_direction += pi_2;

      
      if(related_ball_direction > M_PI_2 && related_ball_direction < M_PI + M_PI_2)
      {
         alpha = M_PI + beta - ball->Get_Direction();
         gamma = beta + alpha;
         
         ball->Set_Direction(gamma);

         return true;
      }
   }
   return false;
}
//------------------------------------------------------------------------------------------------------------

bool AsPlatform::Get_Platform_Image_Stroke_Color(int x, int y, AColor &color, int &stroke_length)
{
   int i;
   int offset = y * Normal_Platform_Image_Width + x;
   int image_color = 0;

   stroke_length = 0;
   
   if(y >= Normal_Platform_Image_Height)
      return false;

   for(i = y; i < Normal_Platform_Image_Height; ++i)
   {
      if(i == y)
      {
         image_color = Normal_Platform_Image[offset];
         ++stroke_length;
      }
      else
      {
         if(Normal_Platform_Image[offset] == image_color)
         {
            ++stroke_length;
         }
         else
            break;
      }

      offset += Normal_Platform_Image_Width;
   }

   if(image_color == Highlight_Color.Get_RGB())
      color.Set_Pen(AsPlatform::Highlight_Color.Get_Pen()); 

   else if(image_color == Platform_Circle_Color.Get_RGB())
      color.Set_Pen(AsPlatform::Platform_Circle_Color.Get_Pen()); 
   
   else if(image_color == Platform_Inner_Color.Get_RGB())
      color.Set_Pen(AsPlatform::Platform_Inner_Color.Get_Pen()); 

   else if(image_color == AsConfig::BG_Color.Get_RGB())
       color.Set_Pen(AsConfig::BG_Color.Get_Pen()); 
      
   return true;
}

double AsPlatform::Get_Platform_Width()
{
   if (Platform_State == EPlatform_State::Rolling && Platform_State.Rolling == EPlatform_Substate_Rolling::Roll_In)
      return (double)Circle_Size;

   else if (Platform_State == EPlatform_State::Expanding)
      return Platform_Expanding.Expanding_Platform_Width;

   else
      return (double)Normal_Width;
}
//------------------------------------------------------------------------------------------------------------
