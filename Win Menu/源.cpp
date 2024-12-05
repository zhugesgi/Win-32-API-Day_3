#include <windows.h>
#include <stdio.h>
#include "resource.h"
//��Ϣ������
HINSTANCE g_hInstance=0;
void OnCreate(HWND hWnd) {
	HMENU hMenu = LoadMenu(g_hInstance, (char*)IDR_MENU1);
	SetMenu(hWnd, hMenu);//���ز˵�
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

							//���        ��ϢID          ��������     ��������
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
		//PostQuitMessage(0);//����ʹGetMessage()��������0����ֹ��Ϣѭ��
		PostMessage(hWnd, WM_QUIT, 0, 0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);//Ĭ�ϴ��ڴ�����,��������Ϣ�������ṩĬ�ϵĴ���ʽ
}
//������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	g_hInstance = hInstance;
	//ע�ᴰ����
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(WNDCLASSEX);  // ??ȱ����һ�е��³������??

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.lpszMenuName = NULL;//(char*)IDR_MENU1;
	wc.lpszClassName = "MyWindowClass";
	wc.lpfnWndProc = WndProc;  //����Ĵ��ڴ�����������
	wc.hInstance = hInstance;

	RegisterClassEx(&wc);//��������ע�ᵽϵͳ

	//��������
	HWND hWnd = CreateWindowEx(0, "MyWindowClass", "MyWindow", WS_OVERLAPPEDWINDOW, 200, 200, 640, 480, NULL, NULL, hInstance, NULL);
	//��ʾ����
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//��Ϣѭ��
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
