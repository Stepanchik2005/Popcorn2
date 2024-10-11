#include "Monster_Set.h"
AsMonster_Set::~AsMonster_Set()
{
   for(auto *it : Monsters)
      delete it;
   
   Monsters.erase(Monsters.begin(), Monsters.end());
}
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
   : Current_Gate_Index(0), Border(0), Is_Frozen(false)
{
  
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Act()
{
   int current_monster_count = 0;

   switch(Monster_Set_State)
   {
   case EMonster_Set_State::Idle:
         break;

   case EMonster_Set_State::Selecting_Next_Gate: 
      if(Is_Frozen)
         break;

      for(auto *monster : Monsters) // проверка не превышен ли лимит выпущенных монстров
           if(! monster->Is_Finished() )
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

   if(Monsters.size() != 0) // очищаем массив монстров, если закончилась их анимация
   {
      auto monster = Monsters.begin();
      while(monster != Monsters.end())
         if( (*monster)->Is_Finished() )
         {
            delete *monster;
            monster = Monsters.erase(monster);
         }
         else
            monster++;
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
   for(auto *monster : Monsters)
      if(monster->Check_Hit(next_x_pos, next_y_pos, ball) )
         return true;

   return false;
}
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos)
{
   for(auto *monster : Monsters)
      if(monster->Check_Hit(next_x_pos, next_y_pos) )
         return true;

   return false;
}
bool AsMonster_Set::Check_Hit(RECT &rect)
{
   for(auto *monster : Monsters)
      if(monster->Check_Hit(rect) )
         return true;

   return false;
}

//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
   double x_pos, y_pos;
   bool gate_pos_index;

   if(Is_Frozen)
      return;

	if(gate_index < 0 || gate_index > AsConfig::Gates_Count)
		AsConfig::Throw();

   AMonster *new_monster = 0;

   if(Monsters.size() >= Max_Count_Monsters)
     return;

  if(AsCommon::Rand(2) == 0)
     new_monster = new AMonster_Comet();
  else
     new_monster = new AMonster_Eye();
  
   Monsters.push_back(new_monster);
     
   if(new_monster == 0)
      return; // все монстры заняты (уже на поле)

   if(gate_index % 2 == 0)
      gate_pos_index = true;
   else
      gate_pos_index = false;

   Border->Get_Gate_Pos(gate_index, x_pos, y_pos);

   new_monster->Activate(x_pos, y_pos + 1.0 / 3.0, gate_pos_index);
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Activate()
{
   Monster_Set_State = EMonster_Set_State::Selecting_Next_Gate;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Destroy_All()
{
   for(auto *monster : Monsters)
         monster->Destroy();

   Monster_Set_State = EMonster_Set_State::Idle;
}
void AsMonster_Set::Set_Freeze_State(bool is_freeze)
{
   Is_Frozen = is_freeze;

   for(auto *monster : Monsters)
       monster->Set_Freeze_State(Is_Frozen);
}
bool AsMonster_Set::Are_All_Destroyed()
{
   if(Monsters.size() != 0)
      return false;
   else
      return true;
}
bool AsMonster_Set::Get_Next_Game_Object(int &index, AGame_Object **obj)
{
   AMonster *monster = 0;

	if(index < 0 || index >= Monsters.size())
      return false;
   
   if(index < Monsters.size())
   {
     *obj = Monsters[index++];
     return true;
   }
   else 
      return false;

}
 