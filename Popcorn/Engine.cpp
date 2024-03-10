#include "Engine.h"
// AsBall_Set




// AsEngine
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
: Game_State(EGS_Lost_Ball),Rest_Distance(0.0), Life_Count(5), Movers{}, Graphics_Objects{}
{
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hwnd)
{// Настройка игры при старте
	SYSTEMTIME sys_time;
	FILETIME file_time;

	GetSystemTime(&sys_time);
	SystemTimeToFileTime(&sys_time, &file_time);

	srand(file_time.dwLowDateTime);

	AsConfig::Hwnd = hwnd;

	AActive_Brick_Red_Blue::Setup_Colors();

	Level.Init();
	Platform.Init(&Ball_Set);
	AFalling_Letter::Init();

	ABall::Add_Hit_Checker(&Border);
	ABall::Add_Hit_Checker(&Level);
	ABall::Add_Hit_Checker(&Platform);

	Level.Set_Current_Level(ALevel::Level_01);

	/*Ball.Set_State(EBS_Normal, Platform.X_Pos + Platform.Width / 2);*/

	/*Platform.Set_State(EPS_Glay_Init);*/

	Platform.Redraw_Platform();

	memset(Movers, 0, AsConfig::Max_Movers_Count);

	Movers[0] = &Platform;
	Movers[1] = &Ball_Set;

	memset(Graphics_Objects, 0, AsConfig::Max_Graphics_Objects_Count);

	Graphics_Objects[0] = (AGraphics_Objects*)&Level;
	Graphics_Objects[1] = (AGraphics_Objects*)&Border;
	Graphics_Objects[2] = (AGraphics_Objects*)&Platform;
	Graphics_Objects[3] = (AGraphics_Objects*)&Ball_Set;

	SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры
	int i;

	SetGraphicsMode(hdc, GM_ADVANCED);

	for(i = 0; i < AsConfig::Max_Graphics_Objects_Count;++i)
		if(Graphics_Objects[i] != 0)
			Graphics_Objects[i]->Clear(hdc, paint_area);
		
		

	for(i = 0; i < AsConfig::Max_Graphics_Objects_Count;++i)
		if(Graphics_Objects[i] != 0)
			Graphics_Objects[i]->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key(EKey_Type key_type, bool key_down)
{
	switch (key_type)
	{
	case EKT_Left:
		Platform.Move(true, key_down);
		break;
	
	case EKT_Right:
		Platform.Move(false, key_down);
		break;

	case EKT_Space:
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
	case EGS_Test_Ball:

		Ball_Set.Set_For_Test();
		Game_State = EGS_Play_Level;
		break;


	case EGS_Play_Level:
		Play_Level();
		break;


	case EGS_Lost_Ball:
		if (Platform.Get_State() == EPS_Missing)
		{
			Game_State = EGS_Restart_Level;
			Platform.Set_State(EPS_Roll_In);
		}
		break;


	case EGS_Restart_Level:
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
		if(Movers[i] != 0)
		{
			Movers[i]->Start_Movement();

			mover_speed = Movers[i]->Get_Speed();

			if(fabs(mover_speed) > max_speed)
				max_speed = fabs(mover_speed);
		}

	Rest_Distance = max_speed;

	// 2. Продвигаем все объекты 
	while(Rest_Distance > 0.0)
	{
		for(i = 0; i < AsConfig::Max_Movers_Count;++i)
			if(Movers[i] != 0)
				Movers[i]->Advance(max_speed);
	
		Rest_Distance -= AsConfig::Moving_Size_Step;
	}

	// 3. Заказываем перерисовку для всех Mover
	for(i = 0; i < AsConfig::Max_Movers_Count;++i)
		if(Movers[i] != 0)
			Movers[i]->End_Movement();

	
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Play_Level()
{
	int lost_ball_count = 0;
	
	Advance_Mover();
	
	lost_ball_count = Ball_Set.Count_Lost_Ball();
			
	if(lost_ball_count == AsConfig::Max_Balls_Count)
	{
		Game_State = EGS_Lost_Ball;

		Level.Stop();

		Platform.Set_State(EPS_Pre_Meltdown);
	}
	else
		Ball_Set.Accelerate();
	
	if (Ball_Set.Is_Test_Finished() )
		Game_State = EGS_Test_Ball;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Restart_Level()
{
	if (Platform.Get_State() == EPS_Ready)
		{
			Game_State = EGS_Play_Level;
			Ball_Set.Set_On_Platform(Platform.Get_Middle_Pos());
			/*Platform.Set_State(EPS_Glay_Init);*/
		}
}
//------------------------------------------------------------------------------------------------------------

void AsEngine::Act()
{
	int i;
	int index = 0;
	
	AFalling_Letter *falling_letter;

	Level.Act();
	Platform.Act();

	if(Platform.Get_State() != EPS_Ready)
		Ball_Set.Act();

	while(Level.Get_Next_Falling_Letter(index, &falling_letter))
	{
		if(Platform.HitBy(falling_letter))
			On_Falling_Letter(falling_letter);
	}

	// Анимация для всех граф объектов по таймеру
	/*for(i = 0; i < AsConfig::Max_Graphics_Objects_Count;++i)
		if(Graphics_Objects[i] != 0)
			Graphics_Objects[i]->Act();*/
}
void AsEngine::On_Falling_Letter(AFalling_Letter *falling_letter)
{
	if(falling_letter->Falling_Letter_State != EFLS_Normal)
		return;

	switch(falling_letter->Letter_Type)
	{
	case ELT_O: 
		Platform.Set_State(EPS_Glay_Finalize); // только для отмены клея	
		break;
	case ELT_I:
		Ball_Set.Inverse_Balls();
		break;

	case ELT_C:  
		Ball_Set.Reset_Speed();
		break;

	case ELT_M:  
		
		break;
	case ELT_G:  
		if(Life_Count < AsConfig::Max_Life_Count)
			++Life_Count; // !!! Добавить метод добавления жизни
		break;
	case ELT_K: 
      Platform.Set_State(EPS_Glay_Init);
		break;
	case ELT_Sh:     
		  
		break;
	case ELT_P:  
		AsConfig::Level_Has_Floor = true;
		Border.Redraw_Floor();
		break;
	case ELT_L:   

		break;
	case ELT_T:   
		Ball_Set.Triple_Balls();
		break;

	/*case ELT_Plus:
		
		break;
	
	default:
		AsConfig::Throw();*/
	}

		falling_letter->Finalize();
}
//------------------------------------------------------------------------------------------------------------
