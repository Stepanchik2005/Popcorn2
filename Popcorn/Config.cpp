#include "Config.h"



//------------------------------------------------------------------------------------------------------------
// AColor
AColor::~AColor()
{
	/*if(Pen != 0)
		DeleteObject(Pen);

	if(Brush != 0)
		DeleteObject(Brush);*/
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor()
: R(0), G(0), B(0), Pen(0), Brush(0)
{
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b)
: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, 0, RGB(r,g,b));
	Brush = CreateSolidBrush(RGB (r,g,b));
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(const AColor &color, int pen_size)
: R(color.R), G(color.G), B(color.B), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, color.Get_RGB());
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(const AColor &pen_color, int pen_size, const AColor &brush_color)
: R(0), G(0), B(0), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, pen_color.Get_RGB());
	Brush = CreateSolidBrush(brush_color.Get_RGB());
}
//------------------------------------------------------------------------------------------------------------
AColor::AColor(unsigned char r, unsigned char g, unsigned char b,int pen_size)
: R(r), G(g), B(b), Pen(0), Brush(0)
{
	Pen = CreatePen(PS_SOLID, pen_size, RGB(r, g, b));
}
//------------------------------------------------------------------------------------------------------------
void AColor::operator=(const AColor& other_color)
{
	AsConfig::Throw(); // присваевам методом Set_As(unsigned char r, unsigned char g, unsigned char b)
}
void AColor::Set_As(unsigned char r, unsigned char g, unsigned char b)
{
	this->R = r;
	this->G = g;
	this->B = b;

	// если нужно очищаем ресурсы
	 if (Pen != 0)
         DeleteObject(Pen);
    if (Brush != 0)
         DeleteObject(Brush);

	 Pen = CreatePen(PS_SOLID, 0, RGB(R, G, B));
	 Brush = CreateSolidBrush(RGB (R, G, B));
	
}
//------------------------------------------------------------------------------------------------------------
void AColor::Select(HDC hdc) const
{
	SelectObject(hdc, Pen);
	SelectObject(hdc, Brush); 
}
//------------------------------------------------------------------------------------------------------------
HBRUSH AColor::Get_Brush() const
{
	return Brush;
}
//------------------------------------------------------------------------------------------------------------
HPEN AColor::Get_Pen() const
{
	return Pen;
}

//------------------------------------------------------------------------------------------------------------
int AColor::Get_RGB() const
{
	return RGB(R, G, B);
}
//------------------------------------------------------------------------------------------------------------

void AColor::Set_Brush(HBRUSH brush)
{
	Brush = brush;
}
//------------------------------------------------------------------------------------------------------------

void AColor::Set_Pen(HPEN pen)
{
	Pen = pen;
}
void AColor::Select_Pen(HDC hdc) const
{
	SelectObject(hdc, Pen);
}





//------------------------------------------------------------------------------------------------------------

void AsConfig::Throw()
{
	throw 13;
}



AColor_Fade::AColor_Fade()
{
}
AColor_Fade::~AColor_Fade()
{
	for(auto it : Fading_Colors)
	    delete it;

	Fading_Colors.erase(Fading_Colors.begin(), Fading_Colors.end());

}
AColor_Fade::AColor_Fade(const AColor &origin_color, int max_fade_step)
	: Max_Fade_Step(max_fade_step)
{
	int i;
	AColor *curr_color;
	for (i = 0; i < Max_Fade_Step; i++)
	{
		curr_color = AsCommon::Get_Fading_Color(origin_color, i, Max_Fade_Step);
		Fading_Colors.push_back(curr_color);
		
	}
}
AColor_Fade::AColor_Fade(const AColor &origin_color, int max_fade_step, const AColor& dest_color)
	: Max_Fade_Step(max_fade_step)
{
	int i;	
	AColor *curr_color;
	for (i = 0; i < Max_Fade_Step; i++)
	{
		curr_color = AsCommon::Get_Fading_Color(origin_color, dest_color, i, Max_Fade_Step);
		Fading_Colors.push_back(curr_color);
		
	}
}

AColor* AColor_Fade::Get_Color(int index)
{
	if(index < 0 || index > Max_Fade_Step)
		AsConfig::Throw();

	return Fading_Colors[index];
}
void AColor_Fade::Init(const AColor &origin_color, const AColor& dest_color, int max_fade_step)
{
	int i;	
	AColor *curr_color;


	Max_Fade_Step = max_fade_step;
	for (i = 0; i < Max_Fade_Step; i++)
	{
		curr_color = AsCommon::Get_Fading_Color(origin_color, dest_color, i, Max_Fade_Step);
		Fading_Colors.push_back(curr_color);
		
	}
}
// AsConfig
bool AsConfig::Level_Has_Floor = true;
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
const AColor AsConfig::Shadow_Color = AColor(AsConfig::BG_Color, AsConfig::Global_Scale);
const AColor AsConfig::Highlight_Color = AColor(AsConfig::White_Color, AsConfig::Global_Scale);

HWND AsConfig::Hwnd;

const double AsConfig::Moving_Size_Step = 1.0 / AsConfig::Global_Scale;
const double AsConfig::D_Global_Scale = 3.0;
const double AsConfig::Accelerate_Ball_Speed = 1.001;
const double AsConfig::Initial_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;


// AsCommon --------
int AsCommon::Rand(int range)
{// Вычисляет псевдослучайное число в диапазоне [0, .. range - 1]

	return rand() * range / (RAND_MAX + 1);
}
//------------------------------------------------------------------------------------------------------------
void AsCommon::Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius)
{
	int radius = corner_radius * AsConfig::Global_Scale;

	RoundRect(hdc, brick_rect.left, brick_rect.top, brick_rect.right - 1, brick_rect.bottom - 1, radius, radius);
}
void AsCommon::Rect(HDC hdc, int x, int y, int width, int height, const AColor &color)
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
void AsCommon::Rect(HDC hdc, RECT &rect, const AColor &color)
{
	color.Select(hdc);

	Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
void AsCommon::Ellipse(HDC hdc, RECT &rect, const AColor &color)
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
AColor* AsCommon::Get_Fading_Color(const AColor &origin_color, int step, int max_fade_step)
{
	return Get_Fading_Color(origin_color, AsConfig::BG_Color, step, max_fade_step);
}
AColor* AsCommon::Get_Fading_Color(const AColor &origin_color, const AColor &dest_color, int step, int max_fade_step)
{
	unsigned char r, g, b;
	AColor *result_color;

	r = Get_Fading_Channel(origin_color.R, dest_color.R, step, max_fade_step);
	g = Get_Fading_Channel(origin_color.G, dest_color.G, step, max_fade_step);
	b = Get_Fading_Channel(origin_color.B, dest_color.B, step, max_fade_step);

	result_color = new AColor(r, g, b);

	return result_color;
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




//AMessage
AMessage::AMessage(EMessage_Type message_type)
	: Message_Type(message_type)
{
}



// AMessage_Manager
std::queue<AMessage *> AsMessage_Manager::Messages;

void AsMessage_Manager::Add_Message(AMessage *message)
{
	if(message != 0)
		Messages.push(message);
}

bool AsMessage_Manager::Get_Message(AMessage **message)
{
	if(Messages.size() == 0)
		return false;

	*message = Messages.front();
	Messages.pop();

	return true;
}


