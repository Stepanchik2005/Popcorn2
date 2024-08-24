#include "Engine.h"

AsInfo_Panel::AsInfo_Panel()
	: Logo_Pop_Font(0), Logo_Corn_Font(0)
{
	Choose_Font();
}
void AsInfo_Panel::Advance(double max_speed)
{

}
double AsInfo_Panel::Get_Speed()
{
	return 0.0;
}
void AsInfo_Panel::Start_Movement()
{
}
void AsInfo_Panel::End_Movement()
{
}

void AsInfo_Panel::Act()
{
}
bool AsInfo_Panel::Is_Finished()
{
	return false;
}
void AsInfo_Panel::Choose_Font()
{
    LOGFONT logo_font{}; 
   
 
	 logo_font.lfHeight = -128;
	 logo_font.lfWeight = 900;
	 logo_font.lfOutPrecision = 3;
	 logo_font.lfClipPrecision = 2;
	 logo_font.lfQuality = 1;
	 logo_font.lfPitchAndFamily = 34;
	 wcscpy_s(logo_font.lfFaceName, L"Arial Black");

	 Logo_Pop_Font = CreateFontIndirect(&logo_font); 

	 logo_font.lfHeight = -96;
	 Logo_Corn_Font = CreateFontIndirect(&logo_font); 

}
void AsInfo_Panel::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int logo_x_pos = 212 * scale;
	int logo_y_pos =  0;
	int shadow_x_offset = 5 * scale;
	int shadow_y_offset = 5 * scale;
   const wchar_t *pop = L"POP";
	const wchar_t *corn = L"CORN";


	AsCommon::Rect(hdc, 211, 5, 104, 100, AsConfig::Blue_Color);

	SetBkMode(hdc, TRANSPARENT);

	SelectObject(hdc, Logo_Pop_Font);

	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB()); // POP
	TextOutW(hdc, logo_x_pos + shadow_x_offset, logo_y_pos + shadow_y_offset, pop, 3);

	SetTextColor(hdc, AsConfig::Red_Color.Get_RGB()); // POP
	TextOutW(hdc, logo_x_pos, logo_y_pos, pop, 3);
	


	SelectObject(hdc, Logo_Corn_Font);

	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB()); //CORN
	TextOutW(hdc, logo_x_pos + shadow_x_offset - 5 * scale, logo_y_pos + shadow_x_offset + 48 * scale, corn, 4);
	SetTextColor(hdc, AsConfig::Red_Color.Get_RGB()); //CORN
	TextOutW(hdc, logo_x_pos - 5 * scale, logo_y_pos + 48 * scale, corn, 4);

	AsCommon::Rect(hdc, 208, 108, 110, 100, AsConfig::Red_Color);
}
void AsInfo_Panel::Clear(HDC hdc, RECT &paint_area)
{
}
void AsInfo_Panel::Draw_Logo()
{
	
}
// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
	: Game_State(EGame_State::Lost_Ball), Rest_Distance(0.0), Life_Count(5), Modules{}, Timer_ID(WM_USER + 1)
{
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hwnd) 
{// Настройка игры при старте
	int index = 0;
	SYSTEMTIME sys_time;
	FILETIME file_time;

	GetSystemTime(&sys_time);
	SystemTimeToFileTime(&sys_time, &file_time);

	srand(file_time.dwLowDateTime);

	AsConfig::Hwnd = hwnd;

	AActive_Brick_Red_Blue::Setup_Colors();
	AExplodive_Ball::Setup_Colors();

	Level.Init();
	Platform.Init(&Ball_Set, &Laser_Beam_Set); 
	AFalling_Letter::Init();
	Monster_Set.Init(&Border);

	ABall::Hit_Checker_List.Add_Hit_Checker(&Border);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Level);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Platform);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

	ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Level);
	ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

	AsPlatform::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

	Level.Set_Current_Level(ALevel::Level_01);

	Platform.Redraw_Platform();

	memset(Modules, 0, AsConfig::Max_Modules_Count);

	Add_New_Module(index, &Level);
	Add_New_Module(index, &Border);
	Add_New_Module(index, &Platform);
	Add_New_Module(index, &Ball_Set);
	Add_New_Module(index, &Laser_Beam_Set);
	Add_New_Module(index, &Monster_Set);
	Add_New_Module(index, &Info_Panel);

	SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры
	int i;

	SetGraphicsMode(hdc, GM_ADVANCED);

	// очистка экрана за кадр
	for(i = 0; i < AsConfig::Max_Modules_Count;++i)
		if(Modules[i] != 0)
			Modules[i]->Clear(hdc, paint_area);
		
		
	// отрисовка экрана за кадр
	for(i = 0; i < AsConfig::Max_Modules_Count;++i)
		if(Modules[i] != 0)
			Modules[i]->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key(EKey_Type key_type, bool key_down)
{
	switch (key_type)
	{
	case EKey_Type::Left:
		Platform.Move(true, key_down);
		break;
	
	case EKey_Type::Right:
		Platform.Move(false, key_down);
		break;

	case EKey_Type::Space:
		if(Ball_Set.Balls[0].Get_State() != EBall_State::Disabled)
			Platform.On_Space_Key(key_down);
	 break;
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{
	
	++AsConfig::Current_Timer_Tick;

	switch (Game_State)
	{
	case EGame_State::Test_Ball:

		Ball_Set.Set_For_Test();
		Game_State = EGame_State::Play_Level;
		break;


	case EGame_State::Play_Level:
		Play_Level();
		break;


	case EGame_State::Lost_Ball:
		if (Platform.Has_Substate_Regular(EPlatform_Substate_Regular::Missing) )
		{
			Game_State = EGame_State::Restart_Level;
			Restart_Level();
		}
		
		break;


	case EGame_State::Restart_Level:
		if(Platform.Has_Substate_Regular(EPlatform_Substate_Regular::Ready))
		{
		  Game_State = EGame_State::Play_Level;
		  Ball_Set.Set_On_Platform(Platform.Get_Middle_Pos());
		  Monster_Set.Activate();
		}
		if (Border.Is_Gate_Opened(AsConfig::Gates_Count - 1) )
			Platform.Set_State(EPlatform_State::Rolling);
		break;
	}
	
	Act();

	//if (AsConfig::Current_Timer_Tick % 10 == 0)

	return 0;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Advance_Mover()
{
	int i;
	double max_speed = 0.0;
	double mover_speed;
	// 1. Определяем макс скорость среди всех объектов
	for(i = 0; i < AsConfig::Max_Movers_Count;++i)
		if(Modules[i] != 0)
		{
			Modules[i]->Start_Movement();

			mover_speed = Modules[i]->Get_Speed();

			if(fabs(mover_speed) > max_speed)
				max_speed = fabs(mover_speed);
		}

	Rest_Distance = max_speed;

	// 2. Продвигаем все объекты 
	while(Rest_Distance > 0.0)
	{
		for(i = 0; i < AsConfig::Max_Movers_Count;++i)
			if(Modules[i] != 0)
				Modules[i]->Advance(max_speed);
	
		Rest_Distance -= AsConfig::Moving_Size_Step;
	}

	// 3. Заказываем перерисовку для всех Mover
	for(i = 0; i < AsConfig::Max_Movers_Count;++i)
		if(Modules[i] != 0)
			Modules[i]->End_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Play_Level()
{
	int lost_ball_count = 0;
	// проверка надо ли остановить уровень
	lost_ball_count = Ball_Set.Count_Lost_Ball();
			
	if(lost_ball_count == AsConfig::Max_Balls_Count)
	{
		Game_State = EGame_State::Lost_Ball;
		Level.Stop();
		Monster_Set.Destroy_All();
		Laser_Beam_Set.Disable_All();
		Platform.Set_State(EPlatform_State::Meltdown);
	}
	else
		Ball_Set.Accelerate();
	
	Advance_Mover();

	if (Ball_Set.Is_Test_Finished() )
		Game_State = EGame_State::Test_Ball;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Restart_Level()
{
	
	if(Border.Is_Gate_Closed(AsConfig::Gates_Count - 1))
 		Border.Open_Gate(AsConfig::Gates_Count - 1, true);

	
}
//------------------------------------------------------------------------------------------------------------

void AsEngine::Act()
{
	int index = 0;
	int i;
	AFalling_Letter *falling_letter;

	// Анимация для всех граф объектов по таймеру
	for(i = 0; i < AsConfig::Max_Modules_Count;++i)
		if(Modules[i] != 0)
			Modules[i]->Act();

	while(Level.Get_Next_Falling_Letter(index, &falling_letter))
	{
		if(Platform.HitBy(falling_letter))
			On_Falling_Letter(falling_letter);
	}

}
void AsEngine::On_Falling_Letter(AFalling_Letter *falling_letter)
{
	if(falling_letter->Falling_Letter_State != EFalling_Letter_State::Normal)
		return;

	switch(falling_letter->Letter_Type)
	{
	case ELetter_Type::O: 
		Platform.Set_State(EPlatform_Substate_Regular::Normal); // только для отмены клея	
		break;
	case ELetter_Type::I:
		Ball_Set.Inverse_Balls();
		Platform.Set_State(EPlatform_Substate_Regular::Normal);
		break;

	case ELetter_Type::C:  
		Ball_Set.Reset_Speed();
		Platform.Set_State(EPlatform_Substate_Regular::Normal);
		break;

	case ELetter_Type::M:  
		
		break;
	case ELetter_Type::G:  
		if(Life_Count < AsConfig::Max_Life_Count)
			++Life_Count; // !!! Добавить метод добавления жизни
		Platform.Set_State(EPlatform_Substate_Regular::Normal);
		break;

	case ELetter_Type::K: 
      Platform.Set_State(EPlatform_State::Glue);
		break;

	case ELetter_Type::Sh:     
		 Platform.Set_State(EPlatform_State::Expanding);
		break;

	case ELetter_Type::P:  
		AsConfig::Level_Has_Floor = true;
		Border.Redraw_Floor();
		Platform.Set_State(EPlatform_Substate_Regular::Normal);
		break;

	case ELetter_Type::L:   
 		Platform.Set_State(EPlatform_State::Laser);
		break;

	case ELetter_Type::T:   
		Platform.Set_State(EPlatform_Substate_Regular::Normal);
		Ball_Set.Triple_Balls();
		break;

	case ELetter_Type::Plus:
		
		break;
	
	default:
		AsConfig::Throw();
	}

		falling_letter->Finalize();
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Add_New_Module(int &index, AGame_Object *obj)
{
	if(index >= 0 && index < AsConfig::Max_Modules_Count)
		Modules[index++] = obj;
	else
		AsConfig::Throw();	
}
