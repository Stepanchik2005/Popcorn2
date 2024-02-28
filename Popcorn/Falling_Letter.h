#pragma once
#include "Active_Brick.h"

enum EFalling_Letter_State
{
	EFLS_Normal,
	EFLS_Finilazing, // Начинаем удалять объект
	EFLS_Finished // Когда объект можно удалять
};
enum ELetter_Type
{
	
	ELT_O, //О ("Отмена") — отмена действия символов К, Ш, П, Л и М. V
	ELT_I, // И ("Инверсия") V
	ELT_C, // С ("Скорость") V
	ELT_M, //  М ("Монстры")
	ELT_G, // Ж ("Жизнь") V
	ELT_K, // К ("Клей")
	ELT_Sh, // Ш ("Шире")	
	
	ELT_P, //  П ("Пол")
	ELT_L, //  Л ("Лазер")
	ELT_T, // Т ("Три")	

	ELT_Plus, // + (Переход на следующий уровень)
	ELT_Max
};
		
class AFalling_Letter : public AGraphics_Objects
{
public:
	AFalling_Letter(ELetter_Type letter_type, int x, int y, EBrick_Type brick_type);
   
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Act();
	virtual bool Is_Finished();
  	virtual void Clear(HDC hdc, RECT &paint_area);

	void Get_Letter_Cell(RECT& rect);
	void Finalize();
	void Test_Draw_Letter(HDC hdc);

	static void Init();
	static ELetter_Type Get_Random_Letter_Type();
	const ELetter_Type Letter_Type;
	const EBrick_Type Brick_Type;
	EFalling_Letter_State Falling_Letter_State;
	

private:
	void Draw_Brick_Letter(HDC hdc);
	void Set_Brick_Letter_Colors(bool is_switch_color, const AColor **front_color, const AColor **back_color);
	void Draw_Line(HDC hdc, int x_1, int y_1, int x_2, int y_2);
	void Draw_Line_To(HDC hdc,int x, int y);
	void Draw_C(HDC hdc);

	int Next_Rotation_Tick;
	int Rotation_Step;
   int X, Y;	
	RECT Letter_Cell, Prev_Letter_Cell;

	static const int Ticks_Per_Second = 4;
	static const int Max_Rotation_Step = 16;
	static const int Brick_Half_Height = AsConfig::Brick_Height * AsConfig::Global_Scale / 2;
	static int All_Letter_Popularity;

	static int Letters_Popularity[ELT_Max];
};