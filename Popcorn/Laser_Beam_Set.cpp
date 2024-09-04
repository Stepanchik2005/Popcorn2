#include "Laser_Beam_Set.h"


//AsLaser_Beam_Set
void AsLaser_Beam_Set::Init()
{
   memset(Beams,0,sizeof(Beams));
}
void AsLaser_Beam_Set::Fire(double left_gun_x_pos, double right_gun_x_pos)
{
   int i;
   
   ALaser_Beam *left_beam = 0, *right_beam = 0;

   for(i = 0; i < AsLaser_Beam_Set::Max_Count_Beams; ++i)
   {
      if(Beams[i].Is_Active() )
         continue;
       
      if(left_beam == 0)
      {
         left_beam = &Beams[i];
         continue;
      }

      if(right_beam == 0)
      {
         right_beam = &Beams[i];
         break;
      }

   }
   if(left_beam == 0 || right_beam == 0)
      AsConfig::Throw();

  

   left_beam->Set_At(left_gun_x_pos, AsConfig::Platform_Y_Pos - 1); // задаем координаты лучам
   right_beam->Set_At(right_gun_x_pos, AsConfig::Platform_Y_Pos - 1);
}

void AsLaser_Beam_Set::Disable_All()
{
   int i;
   for(i = 0; i < Max_Count_Beams; ++i)
      Beams[i].Disable();
}

bool AsLaser_Beam_Set::Get_Next_Game_Object(int &index, AGame_Object **obj)
{
   if(index >= 0 && index < Max_Count_Beams)
   {
      *obj = &Beams[index++]; // увеличиваем индекс после присваивания 
      return true;
   }
   else
      return false;
}