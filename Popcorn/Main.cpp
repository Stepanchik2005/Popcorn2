// Popcorn.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Main.h"


AsFrame_DC::AsFrame_DC()
	: Width(0), Height(0), DC(0), Bitmap(0)
{
}

HDC AsFrame_DC::Get_DC(HWND hwnd, HDC hdc)
{
	RECT window_rect;
	int dc_weight, dc_height;

	GetClientRect(hwnd, &window_rect);

	dc_weight = window_rect.right - window_rect.left;
	dc_height = window_rect.bottom - window_rect.top;

	if(dc_weight != Width && dc_height != Height)
	{
		if(Bitmap != 0)
			DeleteObject(Bitmap);

		if(DC != 0)
			DeleteObject(DC);

		DC = CreateCompatibleDC(hdc);
		Bitmap = CreateCompatibleBitmap(hdc, dc_weight, dc_height);
		
		Width = dc_weight;
		Height = dc_height;

		SelectObject(DC, Bitmap);

		++window_rect.right;
		++window_rect.bottom;

		AsCommon::Rect(DC, window_rect, AsConfig::BG_Color);

	}

	return DC;
}

// Global Variables:
AsMain_Window Main_Window;

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE previnstance, LPWSTR cmdline, int cmdshow)
{
	Main_Window.Main(instance, cmdshow);
}




//------------------------------------------------------------------------------------------------------------

AsMain_Window *AsMain_Window::Self = 0;
AsMain_Window::AsMain_Window()
	: Engine {},Frame_DC{}, Instance(0), Size_Title{}, Size_Window_Class{}
{
	Self = this;
}
int APIENTRY AsMain_Window::Main(HINSTANCE instance, int cmdshow)
{
	MSG msg;
	HACCEL accel_table;

	// Initialize global strings
	LoadStringW(instance, IDS_APP_TITLE, Size_Title, Max_Load_String);
	LoadStringW(instance, IDC_POPCORN, Size_Window_Class, Max_Load_String);
	My_Register_Class(instance);

	// Perform application initialization:
	if (!Init_Instance(instance, cmdshow))
		return FALSE;

   accel_table = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_POPCORN));


	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, accel_table, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
void AsMain_Window::On_Paint(HWND hwnd)
{
	HDC hdc, frame_dc;
	PAINTSTRUCT ps;
	
	hdc = BeginPaint(hwnd, &ps);
	  
	frame_dc = Frame_DC.Get_DC(hwnd, hdc);

	Engine.Draw_Frame(frame_dc, ps.rcPaint);

	BitBlt(hdc, 0, 0, Frame_DC.Width, Frame_DC.Height, frame_dc, 0, 0, SRCCOPY);

	EndPaint(hwnd, &ps);

}


ATOM AsMain_Window::My_Register_Class(HINSTANCE instance)
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Window_Proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_POPCORN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = AsConfig::BG_Color.Get_Brush();
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_POPCORN);
	wcex.lpszClassName = Size_Window_Class;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL AsMain_Window::Init_Instance(HINSTANCE instance, int cmdshow)
{
	RECT window_rect = {};
	HWND hwnd = {};

	Instance = instance; // Store instance handle in our global variable

	
	window_rect.left = 0;
	window_rect.top = 0;
	window_rect.right = 320 * 3;
	window_rect.bottom = 200 * 3;

	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, TRUE);

	hwnd = CreateWindowW(Size_Window_Class, Size_Title, WS_OVERLAPPEDWINDOW, 0, 0, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0, 0, instance, 0);

	if (hwnd == 0)
		return FALSE;

	Engine.Init_Engine(hwnd);

	ShowWindow(hwnd, cmdshow);
	UpdateWindow(hwnd);

	return TRUE;
}

LRESULT CALLBACK AsMain_Window::Window_Proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	int wmid;
	
	switch (message)
	{
	case WM_COMMAND:
		wmid = LOWORD(w_param);
		// Parse the menu selections:
		switch (wmid)
		{
		case IDM_ABOUT:
			DialogBox(Self->Instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
		}
	break;


	case WM_PAINT:
	{
		Self->On_Paint(hwnd);
		break;
	}


	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	case WM_KEYDOWN:
		switch (w_param)
		{
		case VK_LEFT:
			return Self->Engine.On_Key(EKey_Type::Left, true);

		case VK_RIGHT:
			return Self->Engine.On_Key(EKey_Type::Right, true);

		case VK_SPACE:
			return Self->Engine.On_Key(EKey_Type::Space, true);
		}
		break;

	case WM_KEYUP:
		switch (w_param)
		{
		case VK_LEFT:
			return Self->Engine.On_Key(EKey_Type::Left, false);

		case VK_RIGHT:
			return Self->Engine.On_Key(EKey_Type::Right, false);

		case VK_SPACE:
			return Self->Engine.On_Key(EKey_Type::Space, false);
		}
		break;

	case WM_CHAR:
		Self->Engine.On_Char(w_param);
		break;

	case WM_TIMER:
		if (w_param == Self->Engine.Timer_ID)
			return Self->Engine.On_Timer();
		break;


	default:
		return DefWindowProc(hwnd, message, w_param, l_param);
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------
// Message handler for about box.
INT_PTR CALLBACK AsMain_Window::About(HWND dlg, UINT message, WPARAM w_param, LPARAM l_param)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(w_param) == IDOK || LOWORD(w_param) == IDCANCEL)
		{
			EndDialog(dlg, LOWORD(w_param));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
//------------------------------------------------------------------------------------------------------------
/*

Конец уровня при потере мяча
V 1. Анимация расплавления платформы
V 2. Анимация выкатывания новой
V 3. Анимация расширяющейся платформы
4. Пуск мяча пробелом и состояния мяча

Кирпичи
1. Обычные кирпичи (синие или красные)
1.1. Могут быть с буквой или без
1.2. При попадании в кирпич он исчезает из уровня сразу, и если кирпич был:
1.2.1. Без буквы - создаётся активный кирпич
1.2.2. С буквой - создаётся падающая буква

2. Неразрушаемый кирпич
2.1. Анимация при попадании

3. Многоразовый кирпич
3.1. 4 состояния кирпича (1-4 удара до разрушения)
3.2. Переход по состояниям
3.3. Анимация приза при разрушении (кирпич исчезает, анимация - остаётся)

4. Кирпич с парашютом
4.1. Анимация парашюта - как вариант падающей буквы

5. Кирпич телепортации
5.1. Анимация при захвате и выпуске мяча

6. Кирпич рекламы
6.1. Анимация фрагмента "рекламы", скрывавшейся за кирпичом


Взаимодействие особых кирпичей
1. Падающая буква - взаимодействует только с платформой
2. Активный кирпич - взаимодействует только с шариком


Действия букв
1. Простые:
1.1. О ("Отмена") — отмена действия символов К, Ш, П, Л и М.
1.2. И ("Инверсия")
1.3. С ("Скорость")
1.4. М ("Монстры")
1.5. Ж ("Жизнь")


2. Сложные:
2.1. К ("Клей")
2.1.1. Новое состояние платформы
2.1.2. Анимация растекающегося клея (прямая и обратная)
2.1.3. Фиксация шарика (+ новое состояние шарика)
2.1.4. Пуск шарика пробелом и по таймауту

2.2. Ш ("Шире")
2.2.1. Новое состояние платформы (+ изменение размера)
2.2.2. Анимация расширяющейся платформы (прямая и обратная)

2.3. П ("Пол")
2.3.1. Отрисовка пола
2.3.2. Шкала пола
2.3.3. Взаимодействие пола с мячом
2.3.4. Отбитие мяча нижней гранью платформы

2.4. Л ("Лазер")
2.4.1. Новое состояние платформы
2.4.2. Анимация превращения в пушку (прямая и обратная)
2.4.3. Стрельба пробелом:
2.4.3.1. Обработка пробела особым образом, когда платформа в режиме лазера
2.4.3.2. Объекты лазерных лучей - анимация, перемещение и взаимодействие с другими объектами

2.5. Т ("Три")
2.5.1. Добавить поддержку множества мячиков
2.5.2. Вывод множества мячиков
2.5.3. Взаимодействие множества мячиков с элементами игры

2.6. + (Переход на следующий уровень)
2.6.1. Анимация досрочного перехода на следующий уровень
2.6.2. Отдельный уровень перехода с особыми стенками (рамкой)

Движение мячика
1. Отскок мячика от рамки
2. Попадания в края платформы
3. Подсечка мяча платформой
4. Попадание в монстра

Попадание в кирпич
1. Попадание в кирпич - состояния кирпича
2. Выбивание падающей буквы
3. Перехват падающей буквы
4. Для каждой буквы - свои действия

Гейты
1. Вывод гейтов
2. Анимация открытия/закрытия гейтов
3. Состояния гейтов (закрыт, открывается, закрывается)


Монстры
1. Список монстров (глаз, "черепаха", "сатурн", голова, "бубен", аквариум, зажигалка)
2. Для каждого - сделать раскадровку
3. Перемещение, выбор направления + состояния (живой/пауза/исчезает)
4. Взаимодействие с мячом и платформой
5. Выход из гейта
6. Для каждого - анимация


Информационная панель
1. Логотип
2. Индикаторы
2.1. Имя игрока
2.2 Текущий счёт
3. Шкала монстров и пола
4. Изменение шкал
5. Окно дополнительных жизней
6. Учёт игровых действий и отображение на индикаторах


Игра и уровни
1. Список первых 10 уровней
2. Состояния игры (заставка, анимация начала уровня, играем уровень, потеря жизни, переход на следующий уровень (нормальный и досрочный), окончание игры)


*/

