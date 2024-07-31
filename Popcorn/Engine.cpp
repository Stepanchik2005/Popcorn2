#include "Engine.h"


// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
: Game_State(EGame_State::Lost_Ball),Rest_Distance(0.0), Life_Count(5)
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

	Level.Init();
	Platform.Init(&Ball_Set, &Laser_Beam_Set); 
	AFalling_Letter::Init();
	
	//Laser_Beam_Set.Init();

	ABall::Hit_Checker_List.Add_Hit_Checker(&Border);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Level);
	ABall::Hit_Checker_List.Add_Hit_Checker(&Platform);

	ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Level);
	Level.Set_Current_Level(ALevel::Level_01);

 	/*Ball.Set_State(EBall_State::Normal, Platform.X_Pos + Platform.Width / 2);*/

	/*Platform.Set_State(EPS_Glue_Init);*/
	//Platform.Set_State(EPlatform_State::Laser);

	Platform.Redraw_Platform();

	memset(Modules, 0, AsConfig::Max_Modules_Count);

	/*Modules[0] = &Level;
	Modules[1] = &Border;
	Modules[2] = &Platform;
	Modules[3] = &Ball_Set;
	Modules[4] = &Laser_Beam_Set;*/

	Add_New_Module(index, &Level);
	Add_New_Module(index, &Border);
	Add_New_Module(index, &Platform);
	Add_New_Module(index, &Ball_Set);
	Add_New_Module(index, &Laser_Beam_Set);
	
	Border.Open_Gate(AsConfig::Gates_Count - 3, false);

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
			Game_State = EGame_State::Restart_Level;
		
		break;


	case EGame_State::Restart_Level:
		Restart_Level();
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
	Border.Open_Gate(AsConfig::Gates_Count - 1, true);

	if (Border.Is_Gate_Opened(AsConfig::Gates_Count - 1) )
			Platform.Set_State(EPlatform_State::Rolling);
		
	if(Platform.Has_Substate_Regular(EPlatform_Substate_Regular::Ready))
	{
			Game_State = EGame_State::Play_Level;
			Ball_Set.Set_On_Platform(Platform.Get_Middle_Pos());
	}
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
