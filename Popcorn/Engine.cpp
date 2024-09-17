#include "Engine.h"


// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
	: Game_State(EGame_State::Lost_Ball), Rest_Distance(0.0), Modules{}, Timer_ID(WM_USER + 1)
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

	Level.Init();
	Platform.Init(&Ball_Set, &Laser_Beam_Set); 
	AFalling_Letter::Init();
	Monster_Set.Init(&Border);
	Info_Panel.Init();
	AsMop::Init();

	ABall::Hit_Checker_List.Add_Hit_Checker(&Border);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Level);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Platform);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

	ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Level);
	ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

	AsPlatform::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

	Level.Set_Current_Level(4);

	Platform.Redraw_Platform();

	Modules.push_back(&Level);
	Modules.push_back(&Border);
	Modules.push_back(&Platform);
	Modules.push_back(&Ball_Set);
	Modules.push_back(&Laser_Beam_Set);
	Modules.push_back(&Monster_Set);
	Modules.push_back(&Info_Panel);
	Modules.push_back(&Mop);

	SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры
	SetGraphicsMode(hdc, GM_ADVANCED);

	// очистка экрана за кадр
	for(auto it = Modules.begin(); it != Modules.end(); it++)
			(*it)->Clear(hdc, paint_area);
		
		
	// отрисовка экрана за кадр
	for(auto it = Modules.begin(); it != Modules.end(); it++)
			(*it)->Draw(hdc, paint_area);
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
			if(! Info_Panel.Decrease_Life_Count())
				Game_Over();		

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
		if (Border.Is_Gate_Opened(AsConfig::Gates_Count - 1) && Platform.Get_State() != EPlatform_State::Rolling)
		{
			Platform.Set_State(EPlatform_State::Rolling);
		}

		

		break;
	}
	
	Act();

	//if (AsConfig::Current_Timer_Tick % 10 == 0)

	return 0;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Advance_Mover()
{
	double max_speed = 0.0;
	double mover_speed;
	// 1. Определяем макс скорость среди всех объектов
	for(auto it = Modules.begin(); it != Modules.end();it++)
	{
			(*it)->Start_Movement();

			mover_speed = (*it)->Get_Speed();

			if(fabs(mover_speed) > max_speed)
				max_speed = fabs(mover_speed);
	}
		
	Rest_Distance = max_speed;

	// 2. Продвигаем все объекты 
	while(Rest_Distance > 0.0)
	{
		for(auto it = Modules.begin(); it != Modules.end();it++)
				(*it)->Advance(max_speed);
	
		Rest_Distance -= AsConfig::Moving_Size_Step;
	}

	// 3. Заказываем перерисовку для всех Mover
	for(auto it = Modules.begin(); it != Modules.end();it++)
			(*it)->End_Movement();
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
void AsEngine::Game_Over()
{
	// !!! Надо сделать
}
//------------------------------------------------------------------------------------------------------------

void AsEngine::Act()
{
	int index = 0;
	AFalling_Letter *falling_letter;
	

	// Анимация для всех граф объектов по таймеру
	for(auto it = Modules.begin(); it != Modules.end(); it++)
			(*it)->Act();

	while(Level.Get_Next_Falling_Letter(index, &falling_letter))
	{
		if(Platform.HitBy(falling_letter))
			On_Falling_Letter(falling_letter);
	}

	Handle_Message();
}

void AsEngine::Handle_Message()
{
	AMessage *message;

	if(AsMessage_Manager::Get_Message(&message))
	{
		switch(message->Message_Type)
		{
			case EMessage_Type::Floor_Is_Over:
				AsConfig::Level_Has_Floor = false;
				Border.Redraw_Floor();
				break;

			case EMessage_Type::Unfreeze_Monsters:
				 Monster_Set.Set_Freeze_State(false);
				break;

			default:
				AsConfig::Throw();
		}
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
		Info_Panel.Floor_Indicator.Cancel();
		Info_Panel.Monster_Indicator.Cancel();
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
		Monster_Set.Set_Freeze_State(true);
		Info_Panel.Monster_Indicator.Restart();
		break;
	case ELetter_Type::G:  
		
		Info_Panel.Increase_Life_Count();
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
		Info_Panel.Floor_Indicator.Restart();
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

	AsInfo_Panel::Update_Score(EHit_Type::Catch_Letter);

	falling_letter->Finalize();
}
//------------------------------------------------------------------------------------------------------------


	


