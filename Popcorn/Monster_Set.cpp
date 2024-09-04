#include "Monster_Set.h"
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
   : Current_Gate_Index(0), Border(0)
{
   memset(Monsters, 0, sizeof(AMonster *) * Max_Count_Monsters);
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Act()
{
   int i;
   int current_monster_count = 0;

   switch(Monster_Set_State)
   {
   case EMonster_Set_State::Idle:
         break;

   case EMonster_Set_State::Selecting_Next_Gate: 
      for(i = 0; i < Max_Count_Monsters; ++i) // проверка не превышен ли лимит выпущенных монстров
           if(Monsters[i] != 0 && ! Monsters[i]->Is_Finished() )
              ++current_monster_count;

       if(current_monster_count < Max_Alive_Monsters_Count)
       {// случайным образом выбираем номер гейта

          Current_Gate_Index = Border->Open_Long_Gate();
          Monster_Set_State = EMonster_Set_State::Waiting_Gate_Opening;
       }
    
      break;
      
   case EMonster_Set_State::Waiting_Gate_Opening: 
      if(Border->Is_Gate_Opened(Current_Gate_Index))
      {
         Emit_At_Gate(Current_Gate_Index);
         Monster_Set_State = EMonster_Set_State::Waiting_Gate_Closing;
      }

      break;
      
   case EMonster_Set_State::Waiting_Gate_Closing: 
     if(Border->Is_Gate_Closed(Current_Gate_Index))
          Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
     
      break;
   }

   if(Monster_Set_State != EMonster_Set_State::Idle)
      for(i = 0; i < Max_Count_Monsters; ++i)
         if(Monsters[i] != 0 && Monsters[i]->Is_Finished() )
         {
            delete Monsters[i];
            Monsters[i] = 0;
         }

   AsGame_Objects_Set::Act();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder *border)
{
   Border = border;

}
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
   int i;
   for(i = 0; i < Max_Count_Monsters; ++i)
      if(Monsters[i] != 0 && Monsters[i]->Check_Hit(next_x_pos, next_y_pos, ball) )
         return true;

   return false;
}
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos)
{
   int i;
   for(i = 0; i < Max_Count_Monsters; ++i)
      if(Monsters[i] != 0 && Monsters[i]->Check_Hit(next_x_pos, next_y_pos) )
         return true;

   return false;
}
bool AsMonster_Set::Check_Hit(RECT &rect)
{
   int i;
   for(i = 0; i < Max_Count_Monsters; ++i)
      if(Monsters[i] != 0 && Monsters[i]->Check_Hit(rect) )
         return true;

   return false;
}

//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
   int i;
   double x_pos, y_pos;
   bool gate_pos_index;

	if(gate_index < 0 || gate_index > AsConfig::Gates_Count)
		AsConfig::Throw();

   AMonster *monster = 0;

   for(i = 0; i < Max_Count_Monsters; ++i)
   {
      if(Monsters[i] == 0)
      {
         if(AsCommon::Rand(2) == 0)
            monster = new AMonster_Comet();
         else
            monster = new AMonster_Eye();

         Monsters[i] = monster;
         break;
      }

   }
   if(monster == 0)
      return; // все монстры заняты (уже на поле)

   if(gate_index % 2 == 0)
      gate_pos_index = true;
   else
      gate_pos_index = false;

   Border->Get_Gate_Pos(gate_index, x_pos, y_pos);

   monster->Activate(x_pos, y_pos, gate_pos_index);

   
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Activate()
{
   Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
}
void AsMonster_Set::Destroy_All()
{
   int i;
   for(i = 0; i < Max_Alive_Monsters_Count; ++i)
      if(Monsters[i] != 0)
         Monsters[i]->Destroy();

   Monster_Set_State = EMonster_Set_State::Idle;
}
bool AsMonster_Set::Get_Next_Game_Object(int &index, AGame_Object **obj)
{
   AMonster *monster = 0;

	if(index < 0 || index >= Max_Count_Monsters)
      return false;
	
   while(index < Max_Count_Monsters)
   {
     monster = Monsters[index++];

     if(monster != 0)
     {
	     *obj = monster;
	     return true;
     }
   }

	return false;
}
