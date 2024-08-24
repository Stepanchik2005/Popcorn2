#include "Config.h"

// AsConfig
bool AsConfig::Level_Has_Floor = false;
int AsConfig::Current_Timer_Tick = 0;
const double AsConfig::Ball_Radius = 2.0 - 0.5 / AsConfig::Global_Scale;

const AColor AsConfig::BG_Color(15, 63, 31);
const AColor AsConfig::Red_Color(255, 85, 85);
const AColor AsConfig::Blue_Color(85, 255, 255);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Monster_Dark_Red_Color(200, 0, 32);

const AColor AsConfig::Letter_Color(White_Color,Global_Scale);
const AColor AsConfig::Laser_Color(White_Color, Global_Scale);
const AColor AsConfig::Paraschute_Color(Blue_Color, 3, Red_Color);
const AColor AsConfig::Teleport_Color(Blue_Color, 3, BG_Color);
const AColor AsConfig::Advertisement_Blue_Table_Color(0, 159, 159, Global_Scale);
const AColor AsConfig::Unbreakable_Blue_Hightlight_Color(Blue_Color, Global_Scale); 
const AColor AsConfig::Unbreakable_Red_Hightlight_Color(Red_Color, 2 * Global_Scale);
const AColor AsConfig::Cornea_Color(BG_Color, Global_Scale - 1, White_Color);
const AColor AsConfig::Iris_Color(BG_Color, Global_Scale * 2 / 3, Blue_Color);
const AColor AsConfig::Cornea_Arc_Color(BG_Color, Global_Scale * 2 / 3);
const AColor AsConfig::Explodive_Red_Color(White_Color, 0, Red_Color);
const AColor AsConfig::Explodive_Blue_Color(White_Color, 0, Blue_Color);
const AColor AsConfig::Comet_Color(Monster_Dark_Red_Color, Global_Scale);

HWND AsConfig::Hwnd;

const double AsConfig::Moving_Size_Step = 1.0 / AsConfig::Global_Scale;
const double AsConfig::D_Global_Scale = 3.0;
const double AsConfig::Accelerate_Ball_Speed = 1.001;
const double AsConfig::Initial_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;

//------------------------------------------------------------------------------------------------------------
void AsConfig::Throw()
{
	throw 13;
}



// AsCommon --------
int AsCommon::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / RAND_MAX;
}
//------------------------------------------------------------------------------------------------------------
void AsCommon::Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius)
{
	int radius = corner_radius * AsConfig::Global_Scale;

	RoundRect(hdc, brick_rect.left, brick_rect.top, brick_rect.right - 1, brick_rect.bottom - 1, radius, radius);
}
void AsCommon::Rect(HDC hdc, int x, int y, int width, int height, AColor color)
{
	RECT rect = {};
	const int scale = AsConfig::Global_Scale;
	color.Select(hdc);

	rect.left = x * scale;
	rect.top = y * scale;
	rect.right = rect.left + width * scale;
	rect.bottom = rect.top + height * scale;

	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Rect(HDC hdc, RECT &rect, AColor color)
{
	color.Select(hdc);

	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Ellipse(HDC hdc, RECT &rect, AColor color)
{
	color.Select(hdc);

	::Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Invalidate_Rect(RECT &rect)
{
	InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
unsigned char AsCommon::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step, int max_fade_step)
{
	return color - step * (color - bg_color) / (max_fade_step - 1);
}
//------------------------------------------------------------------------------------------------------------
void AsCommon::Get_Fading_Color(const AColor &origin_color, int step, int max_fade_step, AColor &result_color)
{
	unsigned char r, g, b;

	r = Get_Fading_Channel(origin_color.R, AsConfig::BG_Color.R, step, max_fade_step);
	g = Get_Fading_Channel(origin_color.G, AsConfig::BG_Color.G, step, max_fade_step);
	b = Get_Fading_Channel(origin_color.B, AsConfig::BG_Color.B, step, max_fade_step);

	result_color = AColor(r,g,b);
}
//------------------------------------------------------------------------------------------------------------
bool AsCommon::Reflect_On_Circle(double next_x_pos, double next_y_pos, double x_pos, double y_pos, double width, ABall_Object *ball)
{
   double dx, dy;
   double distance;
   double two_radiuses;
   double radius,ball_x, ball_y;
   double beta, alpha, gamma;
   double related_ball_direction;
   double const pi_2 = 2.0 * M_PI;

   radius = width / 2.0;
   ball_x =  x_pos + radius /*+ platform_ball_x_offset*/; // координат центра шарика платформы
   ball_y = (double)(y_pos + radius);
   
   dx = next_x_pos - ball_x;
   dy = next_y_pos - ball_y;

   distance = sqrt(dx * dx + dy * dy);
   two_radiuses = AsConfig::Ball_Radius + radius;

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
