#pragma once
#include "Monster.h"
#include "Border.h"


enum class EMonster_Set_State: unsigned char
{
	Idle,
	Selecting_Next_Gate,
	Waiting_Gate_Opening,
	Waiting_Gate_Closing
};
class AsMonster_Set: public AHit_Checker,  public AsGame_Objects_Set
{  
public:
	AsMonster_Set();

	virtual void Act();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);

	void Init(AsBorder *border);
	void Emit_At_Gate(int gate_index);
	void Activate();
	void Destroy_All();
private:
	virtual bool Get_Next_Game_Object(int &index, AGame_Object **obj);

	int Current_Gate_Index;

	EMonster_Set_State Monster_Set_State;
	AsBorder *Border; // UNO

	static const int Max_Count_Monsters = 10;
	
	AMonster *Monsters[Max_Count_Monsters];

	static const int Max_Alive_Monsters_Count = 5;
};
