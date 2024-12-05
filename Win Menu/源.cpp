#include <windows.h>
#include <stdio.h>
#include "resource.h"
//消息处理函数
HINSTANCE g_hInstance=0;
void OnCreate(HWND hWnd) {
	HMENU hMenu = LoadMenu(g_hInstance, (char*)IDR_MENU1);
	SetMenu(hWnd, hMenu);//加载菜单
} 

void OnCommand(HWND hWnd, WPARAM wParam) {
	switch (LOWORD(wParam)) {
	case ID_NEW:
		MessageBox(hWnd, "New", "Info", MB_OK);
		break;
	case ID_QUIT:
		MessageBox(hWnd, "Quit", "Info", MB_OK);
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case ID_About:
		MessageBox(hWnd, "About", "Info", MB_OK);
		break;
	}
} 

void OnRButtonUp(HWND hWnd, LPARAM lParam) {
	HMENU hMenu = LoadMenu(g_hInstance, (char*)IDR_MENU1);
	HMENU hPopup = GetSubMenu(hMenu, 0);
	TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_TOPALIGN, LOWORD(lParam), HIWORD(lParam), 0, hWnd, NULL);
}

							//句柄        消息ID          附带参数     附带参数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CONTEXTMENU:
		OnRButtonUp(hWnd, lParam);
		break;
	case WM_COMMAND:
		OnCommand(hWnd, wParam);
		break;
	case WM_CREATE:
		OnCreate(hWnd);
		break;
	case WM_DESTROY:
		//PostQuitMessage(0);//可以使GetMessage()函数返回0，终止消息循环
		PostMessage(hWnd, WM_QUIT, 0, 0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);//默认窗口处理函数,给各种消息处理函数提供默认的处理方式
}
//主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	g_hInstance = hInstance;
	//注册窗口类
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(WNDCLASSEX);  // ??缺少这一行导致程序崩溃??

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.lpszMenuName = NULL;//(char*)IDR_MENU1;
	wc.lpszClassName = "MyWindowClass";
	wc.lpfnWndProc = WndProc;  //保存的窗口处理函数的名字
	wc.hInstance = hInstance;

	RegisterClassEx(&wc);//将窗口类注册到系统

	//创建窗口
	HWND hWnd = CreateWindowEx(0, "MyWindowClass", "MyWindow", WS_OVERLAPPEDWINDOW, 200, 200, 640, 480, NULL, NULL, hInstance, NULL);
	//显示窗口
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//消息循环
	MSG msg = { 0 };
	/*typedef struct tagMSG {
	HWND        hwnd;
	UINT        message;
	WPARAM      wParam;
	LPARAM      lParam;
	DWORD       time;
	POINT       pt;
#ifdef _MAC
	DWORD       lPrivate;
#endif
} MSG, *PMSG, NEAR *NPMSG, FAR *LPMSG;*/
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
