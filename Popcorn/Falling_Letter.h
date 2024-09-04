#pragma once
#include "Active_Brick.h"

enum class EFalling_Letter_State: unsigned char
{
	Normal,
	Finilazing, // Начинаем удалять объект
	Finished // Когда объект можно удалять
};
enum class ELetter_Type: unsigned char
{
	
	O, //О ("Отмена") — отмена действия символов К, Ш, П, Л и М. V
	I, // И ("Инверсия") V
	C, // С ("Скорость") V
	M, //  М ("Монстры")
	G, // Ж ("Жизнь") V
	K, // К ("Клей")
	Sh, // Ш ("Шире")	
	
	P, //  П ("Пол")
	L, //  Л ("Лазер")
	T, // Т ("Три")	

	Plus, // + (Переход на следующий уровень)
	Max
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
	void Show_Fill_Size();
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

	static int Letters_Popularity[(int)ELetter_Type::Max];
};