#include "Laser_Beam_Set.h"


AsLaser_Beam_Set::AsLaser_Beam_Set()
   :Beams(Max_Count_Beams)
{
}

//AsLaser_Beam_Set
void AsLaser_Beam_Set::Fire(double left_gun_x_pos, double right_gun_x_pos)
{


   ALaser_Beam *left_beam = 0, *right_beam = 0;

   for(auto &beam : Beams)
   {
      if(beam.Is_Active() )
         continue;
       
      if(left_beam == 0)
         left_beam = &beam;
         
      else if(right_beam == 0)
      {
         right_beam = &beam;
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
   for(auto &beam : Beams)
      beam.Disable();
}

bool AsLaser_Beam_Set::Get_Next_Game_Object(int &index, AGame_Object **obj)
{
   if(index >= 0 && index < Beams.size())
   {
      *obj = &Beams[index++]; // увеличиваем индекс после присваивания 
      return true;
   }
   else
      return false;
}