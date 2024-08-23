#pragma once

#include "resource.h"
#include "Engine.h"

class AsFrame_DC
{
public:
	AsFrame_DC();

	HDC Get_DC(HWND hwnd, HDC hdc);
	int Width, Height;

private:
	HDC DC;
	HBITMAP Bitmap;

};

class AsMain_Window
{
public:
	AsMain_Window();
	int APIENTRY Main(HINSTANCE instance, int cmdshow);

private:
	ATOM My_Register_Class(HINSTANCE hInstance);
	BOOL Init_Instance(HINSTANCE instance, int nCmdShow);
	void On_Paint(HWND hwnd);
	static LRESULT CALLBACK Window_Proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
	static INT_PTR CALLBACK About(HWND dlg, UINT message, WPARAM w_param, LPARAM l_param);

	static AsMain_Window *Self;
	static const int Max_Load_String = 100;

	AsEngine Engine;
	AsFrame_DC Frame_DC;
	HINSTANCE Instance;                                // current instance
	WCHAR Size_Title[Max_Load_String];                  // The title bar text
	WCHAR Size_Window_Class[Max_Load_String];            // the main window class name

};