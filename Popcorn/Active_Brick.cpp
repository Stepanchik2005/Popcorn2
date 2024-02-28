#include "Active_Brick.h"

//AGraphics_Objects
AGraphics_Objects::~AGraphics_Objects()
{
}




// AActive_Brick
AActive_Brick::AActive_Brick(EBrick_Type brick_type, int level_x, int level_y)
	: Level_X(level_x),Level_Y(level_y), Brick_Type(brick_type), Brick_Rect{}
{
	Brick_Rect.left = (AsConfig::Level_X_Offset + level_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	Brick_Rect.top = (AsConfig::Level_Y_Offset + level_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	Brick_Rect.right = Brick_Rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	Brick_Rect.bottom = Brick_Rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;
}
void AActive_Brick::Get_X_Y(int &x_pos, int &y_pos)
{
	x_pos = Level_X;
	y_pos = Level_Y;
}
void AActive_Brick::Clear(HDC hdc, RECT &paint_area)
{

}




//------------------------------------------------------------------------------------------------------------
// AActive_Brick_Red_Blue

AColor AActive_Brick_Red_Blue::Fading_Blue_Colors[Max_Fade_Step];	
AColor AActive_Brick_Red_Blue::Fading_Red_Colors[Max_Fade_Step];	

//------------------------------------------------------------------------------------------------------------
AActive_Brick_Red_Blue::~AActive_Brick_Red_Blue()
{
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Red_Blue::AActive_Brick_Red_Blue(int level_x, int level_y,EBrick_Type brick_type)
:  AActive_Brick(brick_type,level_x, level_y),Fade_Step(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Act()
{
	if (Fade_Step < Max_Fade_Step - 1)
	{
		++Fade_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Draw(HDC hdc, RECT &paint_area)
{
	AColor *color = 0;

	switch (Brick_Type)
	{
	case EBT_Red:
		color = &AActive_Brick_Red_Blue::Fading_Red_Colors[Fade_Step];	
		break;

	case EBT_Blue:
		color = &AActive_Brick_Red_Blue::Fading_Blue_Colors[Fade_Step];
		break;
	}

	if(color != 0)
	  color->Select(hdc);

	Brick_Rect.left = (AsConfig::Level_X_Offset + Level_X * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	Brick_Rect.top = (AsConfig::Level_Y_Offset + Level_Y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	Brick_Rect.right = Brick_Rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	Brick_Rect.bottom = Brick_Rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

	AsConfig::Round_Rect(hdc, Brick_Rect);
}
//------------------------------------------------------------------------------------------------------------

bool AActive_Brick_Red_Blue::Is_Finished()
{
	if(Fade_Step >= Max_Fade_Step - 1)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Setup_Colors()
{
	int i;

	for (i = 0; i < Max_Fade_Step; i++)
	{
		Get_Fading_Color(AsConfig::Red_Color, i, Fading_Red_Colors[i]);
		Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Colors[i]);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{
	const AColor *color = 0;

	switch (brick_type)
	{
	case EBT_None:
		color = &AsConfig::BG_Color;
		break;

	case EBT_Red:
		color = &AsConfig::Red_Color;
		break;

	case EBT_Blue:
		color = &AsConfig::Blue_Color;
		break;

	case EBT_Unbreakable:
		color = &AsConfig::White_Color;
		break;

	default:
		AsConfig::Throw();
	}
    
	if(color != 0)
	  color->Select(hdc);

	AsConfig::Round_Rect(hdc, brick_rect);
}
//------------------------------------------------------------------------------------------------------------
unsigned char AActive_Brick_Red_Blue::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step)
{
	return color - step * (color - bg_color) / (Max_Fade_Step - 1);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Red_Blue::Get_Fading_Color(const AColor &origin_color, int step, AColor &result_color)
{
	unsigned char r, g, b;

	r = Get_Fading_Channel(origin_color.R, AsConfig::BG_Color.R, step);
	g = Get_Fading_Channel(origin_color.G, AsConfig::BG_Color.G, step);
	b = Get_Fading_Channel(origin_color.B, AsConfig::BG_Color.B, step);

	result_color = AColor(r,g,b);
}
//------------------------------------------------------------------------------------------------------------


// AActive_Brick_Unbreakble



AActive_Brick_Unbreakable::~AActive_Brick_Unbreakable()
{
	DeleteObject(Region);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Unbreakable::AActive_Brick_Unbreakable(int level_x, int level_y)
:  AActive_Brick(EBT_Unbreakable,level_x, level_y), Animation_Step(0), Region{}
{
	Region = CreateRoundRectRgn(Brick_Rect.left, Brick_Rect.top, Brick_Rect.right + 1, Brick_Rect.bottom + 1, 2 * AsConfig::Global_Scale - 1, 2 * AsConfig::Global_Scale - 1);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Act()
{
	if (Animation_Step < Max_Animation_Step)
	{
		++Animation_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int offset = 2 * Animation_Step * scale - AsConfig::Brick_Width * scale;

	AsConfig::White_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, Brick_Rect);

	SelectClipRgn(hdc, Region);

	AsConfig::Unbreakable_Blue_Hightlight_Color.Select_Pen(hdc);

	MoveToEx(hdc, Brick_Rect.left + 5 * scale + offset, Brick_Rect.bottom + scale, 0);
	LineTo(hdc, Brick_Rect.left + 14 * scale - 1 + offset, Brick_Rect.top - 1 * scale);

	AsConfig::Unbreakable_Red_Hightlight_Color.Select_Pen(hdc);

	MoveToEx(hdc, Brick_Rect.left + 6 * scale + offset, Brick_Rect.bottom + scale, 0);
	LineTo(hdc, Brick_Rect.left + 15 * scale - 1 + offset, Brick_Rect.top - 1 * scale);

	SelectClipRgn(hdc, 0);
}
//------------------------------------------------------------------------------------------------------------

bool AActive_Brick_Unbreakable::Is_Finished()
{
	if(Max_Animation_Step >= Animation_Step - 1)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Draw_In_Level(HDC hdc, RECT &brick_rect)
{
	const AColor *color = 0;

	color = &AsConfig::White_Color;

	if(color != 0)
	  color->Select(hdc);

	AsConfig::Round_Rect(hdc, brick_rect);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Multihit
AActive_Brick_Multihit::~AActive_Brick_Multihit()
{
	
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Multihit::AActive_Brick_Multihit(int level_x, int level_y)
:  AActive_Brick(EBT_Unbreakable,level_x, level_y), Rotation_Step(0)
{
	
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Act()
{

	if (Rotation_Step <= Max_Rotation_Step)
	{
		++Rotation_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int step; 
	double rotation_angle, x_ratio;
	RECT zero_rect = {};
	XFORM xform = {}, old_xform = {};
	// 1. Очищаем фон
	AsConfig::BG_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, Brick_Rect);

	////2. Настраиваем матрицу поворотов
		step = Rotation_Step % Steps_Per_Turn;
	   rotation_angle = M_PI_4 / 2.0 * (double)step;
		x_ratio = cos(rotation_angle);
		// Настраиваем матрицу	
		xform.eM11 = (float)x_ratio;
		xform.eM12 = 0.0f;
		xform.eM21 = 0.0f;
		xform.eM22 = 1.0f;
		xform.eDx = (float)Brick_Rect.left + (1.0 - x_ratio) * (float)(AsConfig::Brick_Width * AsConfig::Global_Scale) / 2.0f;
		xform.eDy = (float)Brick_Rect.top;
		GetWorldTransform(hdc, &old_xform);
		SetWorldTransform(hdc, &xform);


	// 3. Выводим "сто"
	AsConfig::Letter_Color.Select_Pen(hdc);
	MoveToEx(hdc, 0 + 1 * scale + 1, 0 + 3 * scale, 0);
	LineTo(hdc,0 + 3 * scale + 1, 0 + 1 * scale);
	LineTo(hdc,0 + 3 * scale + 1, 0 + 6 * scale - 1);

	zero_rect.left = 0 + 5 * scale + 1; // 10
	zero_rect.top = 0 + 1 * scale; // 1
	zero_rect.right = zero_rect.left + 3 * scale + 1; // 13
	zero_rect.bottom = zero_rect.top + 5 * scale; // 6

	AsConfig::Round_Rect(hdc, zero_rect);

	zero_rect.left += 5 * scale ;
	zero_rect.right += 5 * scale;
	AsConfig::Round_Rect(hdc, zero_rect);

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------

bool AActive_Brick_Multihit::Is_Finished()
{
	if(Rotation_Step >= Max_Rotation_Step )
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{
	const int scale = AsConfig::Global_Scale;
	RECT stage_rect = {};
	//1. Рисуем белый фон
	AsConfig::White_Color.Select(hdc);
   AsConfig::Round_Rect(hdc, brick_rect);

	//2. Рисуем красный прямоугольник поверх фона
	AsConfig::Red_Color.Select(hdc);
	Rectangle(hdc, brick_rect.left + scale, brick_rect.top + scale, brick_rect.right - scale - 1, brick_rect.bottom - scale - 1);
	
	//3. Рисуем "жизни" кирпича
	switch(brick_type)
	{
		case EBT_Multihit_1:
		  Draw_Stage(hdc, brick_rect, 2, 10);
				break;

	   case EBT_Multihit_2:
			  Draw_Stage(hdc, brick_rect, 2, 4);
			  Draw_Stage(hdc, brick_rect, 8, 4);
			  break;

		case EBT_Multihit_3:
			 Draw_Stage(hdc, brick_rect, 2, 2);
			 Draw_Stage(hdc, brick_rect, 6, 2);
			 Draw_Stage(hdc, brick_rect, 10,2);
			 break;

		case EBT_Multihit_4:
			 Draw_Stage(hdc, brick_rect, 2, 1);
			 Draw_Stage(hdc, brick_rect, 5, 1);
			 Draw_Stage(hdc, brick_rect, 8, 1);
			 Draw_Stage(hdc, brick_rect, 11,1);
		    break;

	  default:
		 AsConfig::Throw();
	}

	
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw_Stage(HDC hdc, RECT &brick_rect, int x, int width)
{
	const int scale = AsConfig::Global_Scale;
	RECT stage_rect = {};
	
	AsConfig::BG_Color.Select(hdc);
	stage_rect.left = brick_rect.left + (x + 1) * scale;
	stage_rect.top = brick_rect.top + 3 * scale;
	stage_rect.right = stage_rect.left + width * scale + 1;
	stage_rect.bottom = stage_rect.top + 3 * scale - 1;

	Rectangle(hdc, stage_rect.left, stage_rect.top, stage_rect.right, stage_rect.bottom);

	AsConfig::Blue_Color.Select(hdc);
	stage_rect.left = brick_rect.left + x * scale;
	stage_rect.top = brick_rect.top + 2 * scale;
	stage_rect.right = stage_rect.left + width * scale + 1;
	stage_rect.bottom = stage_rect.top + 3 * scale - 1;

	Rectangle(hdc, stage_rect.left, stage_rect.top, stage_rect.right, stage_rect.bottom);
}



// AActive_Brick_Teleport
AActive_Brick_Teleport::~AActive_Brick_Teleport()
{
	
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport::AActive_Brick_Teleport(int level_x, int level_y, ABall *ball, AActive_Brick_Teleport *destination_teleport)
:  AActive_Brick(EBT_Teleport,level_x, level_y), Animation_Step(0), Ball(0), Destination_Teleport(destination_teleport), Teleport_State(ETS_Starting),Release_Direction_State(EDS_Left)
{
	Set_Ball(ball);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Act()
{
	double ball_x_pos = 0, ball_y_pos = 0;\
	double direction;
	/*if(AsConfig::Current_Timer_Tick % 10 != 0)
		return;*/

	if (Animation_Step < Max_Animation_Step)
	{
		++Animation_Step;
		InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
	}
	else
	{
		switch(Teleport_State)
		{
		 case ETS_Starting:
			Animation_Step = 0;
			Teleport_State = ETS_Finishing;
			if(Destination_Teleport != 0)
			{
				 Destination_Teleport->Set_Ball(Ball);
				 Ball = 0;
			}
			break;

		 case ETS_Finishing:
		   Teleport_State = ETS_Done;

			if(Ball != 0)
			{
				
			   switch(Release_Direction_State)
				{
				case EDS_Left:
					ball_x_pos = Get_X_Teleport_Pos(false) - ABall::Radius;
					ball_y_pos = Get_Y_Teleport_Pos(true);
					break;

				case EDS_Up:
					ball_x_pos = Get_X_Teleport_Pos(true);
					ball_y_pos = Get_Y_Teleport_Pos(false) -  ABall::Radius;
					break;

				case EDS_Right:
					ball_x_pos = Get_X_Teleport_Pos(false)  + (double)AsConfig::Brick_Width + ABall::Radius;
					ball_y_pos = Get_Y_Teleport_Pos(true);
					break;

				case EDS_Down:
					ball_x_pos = Get_X_Teleport_Pos(true);
					ball_y_pos = Get_Y_Teleport_Pos(false) + (double)AsConfig::Brick_Height +  ABall::Radius;
					break;
					
				}
				direction = Ball->Get_Direction();
			   Ball->Set_State(EBS_Normal, ball_x_pos, ball_y_pos);	
				Ball->Set_Direction(direction);
				Ball = 0;
				InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
			}
			break;

		 case ETS_Done:
			 break;

		 default:
			 AsConfig::Throw();
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Draw(HDC hdc, RECT &paint_area) // отрисовка анимации телепорта
{
	int step = 0;
	
	switch(Teleport_State)
	{ 
		case ETS_Starting:
			step = Animation_Step;
			break;

		case ETS_Finishing:
			step = AActive_Brick_Teleport::Max_Animation_Step - Animation_Step;
			break;
	}
	Draw_In_Level(hdc, Brick_Rect, step);

	if(Ball != 0)
	  Ball->Draw_Teleporting(hdc, step);
}
//------------------------------------------------------------------------------------------------------------

bool AActive_Brick_Teleport::Is_Finished()
{
	if(Teleport_State == ETS_Done)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Set_Ball(ABall *ball)
{
	double ball_x, ball_y;

	ball_x = Get_X_Teleport_Pos(true);
	ball_y = Get_Y_Teleport_Pos(true);

	
	if(ball != 0)
		ball->Set_State(EBS_Teleporting, ball_x, ball_y);
	Ball = ball;
}
//------------------------------------------------------------------------------------------------------------
double AActive_Brick_Teleport::Get_X_Teleport_Pos(bool is_center)
{	
	double pos;
	pos = (AsConfig::Level_X_Offset + (double)Level_X * AsConfig::Cell_Width) ;
	
	if(is_center)
		pos += (double)AsConfig::Brick_Width / 2.0;

	return pos;
}
//------------------------------------------------------------------------------------------------------------

double AActive_Brick_Teleport::Get_Y_Teleport_Pos(bool is_center)
{	
	double pos;
	pos = (AsConfig::Level_Y_Offset + (double)Level_Y * AsConfig::Cell_Height);

	if(is_center)
		pos += (double)AsConfig::Brick_Height / 2.0;

	return pos;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Draw_In_Level(HDC hdc, RECT &brick_rect, int step)
{
	RECT teleport_rect = {};
	const int scale = AsConfig::Global_Scale;

	
	// 1. Фоновый кирпич
	AsConfig::Red_Color.Select(hdc);
	AsConfig::Round_Rect(hdc, brick_rect);

	// 2. Телепорт
	AsConfig::Teleport_Color.Select(hdc);

	teleport_rect.left = brick_rect.left + 3 * scale + 1;
	teleport_rect.top = brick_rect.top + 1 + 2.0 * step;
	teleport_rect.right = teleport_rect.left + 8 * scale;
	teleport_rect.bottom = teleport_rect.top + (AsConfig::Brick_Height - 1) * scale - 4.0 * step;

	Ellipse(hdc, teleport_rect.left, teleport_rect.top, teleport_rect.right, teleport_rect.bottom);
}
//------------------------------------------------------------------------------------------------------------




// AAdvertisement
AAdvertisement::~AAdvertisement()
{
	int i, j;
	HRGN region;
	for (i = 0; i < Height; i++)
		for(j = 0; j < Width; j++)
		{
			region = Brick_Regions[i * Width + j];
			if(region != 0)
				DeleteObject(region);
		}
	delete[] Brick_Regions;
}
//------------------------------------------------------------------------------------------------------------
AAdvertisement::AAdvertisement(int level_x, int level_y, int width, int height)
	: Level_X(level_x), Level_Y(level_y), Width(width), Height(height), Brick_Regions(0),
	  Ball_Height(Circle_Size * AsConfig::Global_Scale), Ball_Width(Circle_Size * AsConfig::Global_Scale),
	  Ball_Offset(0.0), Falling_Speed(0.2), Acceleration_Step(0.1), Deformation_Ratio(0)
{
	const int scale = AsConfig::Global_Scale;

	int i, j;

	Empty_Region = CreateRectRgn(0,0,0,0);

	Max_Brick_Mask_Size = Width * Height;

	Brick_Regions = new HRGN[Max_Brick_Mask_Size];
	
	memset(Brick_Regions, 0, sizeof(HRGN) * Max_Brick_Mask_Size);

	
	Ad_Rect.left = (AsConfig::Level_X_Offset + Level_X * AsConfig::Cell_Width) * scale;
	Ad_Rect.top = (AsConfig::Level_Y_Offset + Level_Y * AsConfig::Cell_Height) * scale;
	Ad_Rect.right = Ad_Rect.left + ( (Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width) * scale;
	Ad_Rect.bottom = Ad_Rect.top + ( (Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height) * scale;

	Ball_X = Ad_Rect.left + 9 * scale + 1 + (Ball_Width / 2);
	Ball_Y = Ad_Rect.top + 2 * scale + (Ball_Height / 2);

	//for (i = 0; i < Height; i++)
	//	for(j = 0; j < Width; j++)
	//		Show_Under_Brick(Level_X + j, Level_Y + i);
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Act()
{
	///*if(AsConfig::Current_Timer_Tick % 4 != 0)
	//	return;*/
	int i, j;
	RECT ad_rect = {};
	for (i = 0; i < Height; i++)
		for(j = 0; j < Width; j++)
			if(Brick_Regions[i * Width + j] != 0)
			{
				ad_rect.left = Ad_Rect.left + j * AsConfig::Cell_Width * AsConfig::Global_Scale;
				ad_rect.top = Ad_Rect.top + i * AsConfig::Cell_Height * AsConfig::Global_Scale;
				ad_rect.right = ad_rect.left + AsConfig::Cell_Width * AsConfig::Global_Scale;
				ad_rect.bottom = ad_rect.top + AsConfig::Cell_Height * AsConfig::Global_Scale;

				InvalidateRect(AsConfig::Hwnd, &ad_rect, FALSE);
		
			}
	Falling_Speed += Acceleration_Step;
	Ball_Offset = High_Limit_Animation - (int)(Falling_Speed * Falling_Speed);

	if(Ball_Offset <= Low_Limit_Animation + Deformation_Height)
		Deformation_Ratio = (Ball_Offset - Low_Limit_Animation) / Deformation_Height;
	else
		Deformation_Ratio = 1.0;

	if(Ball_Offset > High_Limit_Animation || Ball_Offset < Low_Limit_Animation)
		Acceleration_Step = -Acceleration_Step;

}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Draw(HDC hdc, RECT &paint_area)
{
	int i, j;
	int x, y;
	RECT intersection_rect;
	HRGN region;
	const int scale = AsConfig::Global_Scale;
	double deformation;
	int ball_width, ball_height;
	int cell_width, cell_height;
	int shadow_width, shadow_height;
	POINT points[4] = {
		{Ad_Rect.left, Ad_Rect.top + 15 * scale}, 
		{Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale}, 
		{Ad_Rect.left + 31 * scale - 1, Ad_Rect.top + 15 * scale}, 
		{Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale}
	}; // для заливки белым цветом

	cell_width = AsConfig::Cell_Width * scale;
	cell_height = AsConfig::Cell_Height * scale;

	
	if ( !(IntersectRect(&intersection_rect, &paint_area, &Ad_Rect)) )
		return;

	SelectClipRgn(hdc, Empty_Region);

	for (i = 0; i < Height; i++)
		for(j = 0; j < Width; j++)
			if(Brick_Regions[i * Width + j] != 0)
			{
				region = Brick_Regions[i * Width + j];

				if(region != 0)
					ExtSelectClipRgn(hdc, region, RGN_OR);
			}


	 //6.3 Рамка ( она же и очищает фон)
	AsConfig::BG_Color.Select(hdc);
   AsConfig::Blue_Color.Select_Pen(hdc);

	AsConfig::Round_Rect(hdc, Ad_Rect);

	AsConfig::White_Color.Select(hdc);
	Polygon(hdc, points, 4);

	// 3. Синяя тень
	AsConfig::Blue_Color.Select(hdc);


	shadow_width = Ball_Width - 4 * scale;
	shadow_height = 4 * scale;

	deformation = (int) ( (1.0 - Deformation_Ratio)) * (double)scale * 3.0;

	ball_width = shadow_width + deformation;
	ball_height = shadow_height - deformation;

	x = Ball_X - ball_width / 2;
	y = (Ball_Y - ball_height / 2.0) + Ball_Offset / 5 + 8 * scale;

	Ellipse(hdc, x, y, x + ball_width,  y + ball_height);


	// 4. Синяя кайма
	AsConfig::Advertisement_Blue_Table_Color.Select(hdc);
	MoveToEx(hdc, Ad_Rect.left + 1, Ad_Rect.top + 15 * scale, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale);
	LineTo(hdc, Ad_Rect.left + 30 * scale + 1, Ad_Rect.top + 15 * scale);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale);
	LineTo(hdc, Ad_Rect.left + 1, Ad_Rect.top + 15 * scale);


	// 5. Красный борт
	AsConfig::Unbreakable_Red_Hightlight_Color.Select(hdc);
	MoveToEx(hdc, Ad_Rect.left + scale, Ad_Rect.top + 16 * scale, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 21 * scale);
	LineTo(hdc,Ad_Rect.left + 30 * scale - 1, Ad_Rect.top + 16 * scale);

	// 6. Эллипс 12х12
	

	deformation = (1.0 - Deformation_Ratio) * scale * 3.0;
	ball_width = Ball_Width + deformation;
	ball_height = Ball_Height - deformation;

	x = Ball_X - ball_width / 2;
	y = (Ball_Y - ball_height / 2.0) - Ball_Offset;

	AsConfig::Red_Color.Select(hdc);
	Ellipse(hdc, x, y, x + ball_width,  y + ball_height);

	// 6.2 Блик
	AsConfig::Letter_Color.Select(hdc);
	Arc(hdc,  x + scale + 1, y + scale + 1, x + ball_width - scale, y + ball_height - scale,
		x + 4 * scale, y + scale, x + scale, y + 4 * scale);

  
	SelectClipRgn(hdc, 0);
}
//------------------------------------------------------------------------------------------------------------

bool AAdvertisement::Is_Finished()
{
	/*if(Max_Animation_Step >= Animation_Step - 1)
		return true;
	else*/
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Clear(HDC hdc, RECT &paint_area)
{	 

}

//------------------------------------------------------------------------------------------------------------
void AAdvertisement::Show_Under_Brick(int level_x, int level_y)
{
	int x, y;
	int cell_width, cell_height;
	HRGN region;
	RECT rect;
	x = level_x - Level_X;
	y = level_y - Level_Y;

	if(x < 0 || x >= Width)
		AsConfig::Throw();
	if(y < 0 || y >= Height)
		AsConfig::Throw();

	cell_width = AsConfig::Cell_Width * AsConfig::Global_Scale;
	cell_height = AsConfig::Cell_Height * AsConfig::Global_Scale;

	rect.left = Ad_Rect.left + x * cell_width;
	rect.top = Ad_Rect.top + y * cell_height;
	rect.right = rect.left  + cell_width;
	rect.bottom = rect.top + cell_height;

	region = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);

	Brick_Regions[y * Width + x] = region;
}

bool AAdvertisement::Has_Brick_At(int level_x, int level_y)
{
	if(level_x >= Level_X && level_x < Level_X + Width)
		if(level_y >= Level_Y && level_y < Level_Y + Height)
			return true;

	return false;
}

// AActive_Brick_Ad
AActive_Brick_Ad::~AActive_Brick_Ad()
{
	
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Ad::AActive_Brick_Ad(int level_x, int level_y, AAdvertisement *advertisement)
:  AActive_Brick(EBT_Ad,level_x, level_y), Advertisement(advertisement)
{
	if(Advertisement != 0)
		Advertisement->Show_Under_Brick(Level_X, Level_Y);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Act()
{
	InvalidateRect(AsConfig::Hwnd, &Brick_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Draw(HDC hdc, RECT &paint_area)
{
}
//------------------------------------------------------------------------------------------------------------

bool AActive_Brick_Ad::Is_Finished()
{
		return false;
}

//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Draw_In_Level(HDC hdc, RECT &brick_rect)
{
	const int scale = 3;
	
	int x, y;
	int i;
	x = brick_rect.left;
	y = brick_rect.top;

	// 1. Очищаем фон
	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, brick_rect.left, brick_rect.top, brick_rect.right + scale, brick_rect.bottom + scale);

	for(i = 0; i < 2; ++i)
	{
		AsConfig::Red_Color.Select(hdc);
	   Ellipse(hdc, x, y, x + 7 * scale - 1, brick_rect.bottom - 1);
	   
	   AsConfig::White_Color.Select_Pen(hdc);
	   Arc(hdc, x + scale, y + scale, x + (Circle_Size - 1) * scale- 1, y + (Circle_Size - 1) * scale - 1,
	   	x + 2 * scale, y + scale, x + scale, y + 3 * scale);

		x += 8 * scale;
	}
}
//------------------------------------------------------------------------------------------------------------

