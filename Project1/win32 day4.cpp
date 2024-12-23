#include <windows.h>
#include <stdio.h>
#include<time.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 640
#define ID_DOWN_TIMER 1
#define ID_CHECK_TIMER 2

//struct Block{
//	int space[4][4];
//}block[7][4];

//int g_score = 0;//记录分数
int g_xIndex = 3;//记录随机块所在空间的左上角x
int g_yIndex = 0;//记录随机块所在空间的左上角y

int g_blockIndex = 0;//记录当前的俄罗斯方块
int g_RussBlocks[2][4] = { 0 };
int g_backgroundArray[20][10] = {0};

void StopDown();
void BlockDown(HWND hwnd);
void onChangeProc(HWND hWnd);


int canSquareChange();//判断是否可以改变方块
int canLongSquareChange();//判断是否可以改变长方形方块

void OnPaintText(HDC hdc, PAINTSTRUCT* ps);
void OnPaint(HDC hdc,PAINTSTRUCT* ps);//绘制背景
void DrawBlock(HDC hdc);//绘制俄罗斯方块
void OnCreate();//随机生成俄罗斯方块
void CopyBlock();//复制俄罗斯方块到背景
void OnTimer(HWND hWnd);//定时器
void LeftMove(HWND hwnd);//左移俄罗斯方块
void RightMove(HWND hwnd);//右移俄罗斯方块
void DownMove(HWND hwnd);//下移俄罗斯方块


void change0To4SquareShape(HWND hWnd);//改变0方块为4方块
void changeLongSquareShape(HWND hWnd);//改变长方形方块


int IsGameOver();//判断游戏是否结束
void Check_If_Delete_Row(HWND hWnd);
BOOL Check_Buttom();//判断是否到达底部

//0表示空白，1表示方块, 2表示到达底部

//消息处理函数
//句柄        消息ID          附带参数     附带参数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RETURN:
			SetTimer(hWnd, ID_DOWN_TIMER, 500, NULL);
			SetTimer(hWnd, ID_CHECK_TIMER, 1000, NULL);
			break;
		case VK_LEFT:
			LeftMove(hWnd);
			break;
		case VK_RIGHT:
			RightMove(hWnd);
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			DownMove(hWnd);
			break;
		case VK_SPACE:
			onChangeProc(hWnd);
			break;
		}
	case WM_TIMER:
		switch (wParam) {
		case ID_DOWN_TIMER:
			OnTimer(hWnd);
			break;
		case ID_CHECK_TIMER:
			Check_If_Delete_Row(hWnd);
			break;
		}
		break;
	case WM_CREATE:       //程序第一次创建窗口时调用
		OnCreate();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc, &ps);
		OnPaintText(hdc,&ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);//默认窗口处理函数,给各种消息处理函数提供默认的处理方式
}
//主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MyWindowClass";
	wc.lpfnWndProc = WndProc;  //保存的窗口处理函数的名字
	wc.hInstance = hInstance;

	RegisterClassEx(&wc);//将窗口类注册到系统

	//创建窗口
	HWND hWnd = CreateWindowEx(0, "MyWindowClass", "MyWindow", WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_MINIMIZEBOX&~WS_SIZEBOX, 200, 200, 500, 640, NULL, NULL, hInstance, NULL);
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

void OnPaint(HDC hdc, PAINTSTRUCT* ps) {
	
	//双缓冲区
	HDC hdcMem = CreateCompatibleDC(hdc);
	//创建兼容位图
	HBITMAP hBackmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdcMem, hBackmap);
	//在内存dc上绘制
	DrawBlock(hdcMem);
	//复制到窗口dc
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);
	//释放dc
	DeleteObject(hBackmap);
}
void DrawBlock(HDC hdc) {
	//绘制背景
	Rectangle(hdc, 300, 0, 500, 600);
	HBRUSH secondbrush = CreateSolidBrush(RGB(220,220,220));
	HBRUSH brush = (HBRUSH)SelectObject(hdc, secondbrush);
	Rectangle(hdc, 0, 0, 300, 600);
	SelectObject(hdc, brush);
	DeleteObject(secondbrush);
	//绘制背景
	HBRUSH hbrush=CreateSolidBrush(RGB(0,200,180));
	HBRUSH oldbrush= (HBRUSH)SelectObject(hdc,hbrush);
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 1) {
				Rectangle(hdc, j*30+1, i*30+1, j*30+29, i*30+29);
			}		
			if (g_backgroundArray[i][j] == 2) {
				Rectangle(hdc, j*30+1, i*30+1, j*30+29, i*30+29);
			}
		}
	}
	SelectObject(hdc,oldbrush);
	DeleteObject(hbrush);
}
void OnCreate() {
	srand(time(NULL));
	int shape = rand() % 7;
	g_blockIndex = shape;//随机产生六种状况的方块
	switch (shape) {
	case 0:
		//1 1 0 0
		//0 1 1 0
		g_RussBlocks[0][0] = 1; g_RussBlocks[0][1] = 1; g_RussBlocks[0][2] = 0; g_RussBlocks[0][3] = 0;
		g_RussBlocks[1][0] = 0; g_RussBlocks[1][1] = 1; g_RussBlocks[1][2] = 1; g_RussBlocks[1][3] = 0;

		g_xIndex = 3;
		g_yIndex = 0;
		break;
	case 1:
		//0 1 1 0
		//1 1 0 0
		g_RussBlocks[0][0] = 0; g_RussBlocks[0][1] = 1; g_RussBlocks[0][2] = 1; g_RussBlocks[0][3] = 0;
		g_RussBlocks[1][0] = 1; g_RussBlocks[1][1] = 1; g_RussBlocks[1][2] = 0; g_RussBlocks[1][3] = 0;

		g_xIndex = 3;
		g_yIndex = 0;
		break;
	case 2:
		//0 0 1 0
		//1 1 1 0
		g_RussBlocks[0][0] = 0; g_RussBlocks[0][1] = 0; g_RussBlocks[0][2] = 1; g_RussBlocks[0][3] = 0;
		g_RussBlocks[1][0] = 1; g_RussBlocks[1][1] = 1; g_RussBlocks[1][2] = 1; g_RussBlocks[1][3] = 0;

		g_xIndex = 3;
		g_yIndex = 0;
		break;
	case 3:
		//1 0 0 0
		//1 1 1 0
		g_RussBlocks[0][0] = 1; g_RussBlocks[0][1] = 0; g_RussBlocks[0][2] = 0; g_RussBlocks[0][3] = 0;
		g_RussBlocks[1][0] = 1; g_RussBlocks[1][1] = 1; g_RussBlocks[1][2] = 1; g_RussBlocks[1][3] = 0;

		g_xIndex = 3;
		g_yIndex = 0;
		break;
	case 4:
		//0 0 0 0 1 0 0 0 0 0 
		//0 0 0 1 1 1 0 0 0 0 
		g_RussBlocks[0][0] = 0; g_RussBlocks[0][1] = 1; g_RussBlocks[0][2] = 0; g_RussBlocks[0][3] = 0;
		g_RussBlocks[1][0] = 1; g_RussBlocks[1][1] = 1; g_RussBlocks[1][2] = 1; g_RussBlocks[1][3] = 0;

		g_xIndex = 3;
		g_yIndex = 0;
		break;
	case 5:
		//0 0 0 0 1 1 0 0 0 0
		//0 0 0 0 1 1 0 0 0 0
		g_RussBlocks[0][0] = 0; g_RussBlocks[0][1] = 1; g_RussBlocks[0][2] = 1; g_RussBlocks[0][3] = 0;
		g_RussBlocks[1][0] = 0; g_RussBlocks[1][1] = 1; g_RussBlocks[1][2] = 1; g_RussBlocks[1][3] = 0;

		g_xIndex = 4;
		g_yIndex = 0;
		break;
	case 6:
		//0 0 0 1 1 1 1 0 0 0 
		//0 0 0 0 0 0 0 0 0 0
		g_RussBlocks[0][0] = 1; g_RussBlocks[0][1] = 1; g_RussBlocks[0][2] = 1; g_RussBlocks[0][3] = 1;
		g_RussBlocks[1][0] = 0; g_RussBlocks[1][1] = 0; g_RussBlocks[1][2] = 0; g_RussBlocks[1][3] = 0;

		g_xIndex = 4;
		g_yIndex = 0;
		break;
	default:
		break;
	}
	CopyBlock();
}
void CopyBlock() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			if (g_RussBlocks[i][j] == 1) {
				g_backgroundArray[i][j+3] = g_RussBlocks[i][j];
			}
		}
	}
}
void OnTimer(HWND hWnd) {
	if (!Check_Buttom()) 
	{
		//停止下落
		StopDown();
		//检查是否有行消除
		Check_If_Delete_Row(hWnd);
		//产生新的方块
		if (IsGameOver()) {
			KillTimer(hWnd, ID_DOWN_TIMER);
			KillTimer(hWnd, ID_CHECK_TIMER);
			MessageBox(hWnd, "Game Over!", "Game Over", MB_OK);
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
			return;
		}
		OnCreate();
	}
	else 
	{
		BlockDown(hWnd);
	}
	InvalidateRect(hWnd, NULL, FALSE);//重绘窗口,此时产生WM_PAINT消息,调用OnPaint函数
}
void BlockDown(HWND hwnd) {
	if (!Check_Buttom())
	{
		//停止下落
		StopDown();
		InvalidateRect(hwnd, NULL, FALSE);
		//检查是否有行消除
		Check_If_Delete_Row(hwnd);
		//产生新的方块
		OnCreate();
		InvalidateRect(hwnd, NULL, FALSE);
	}
	else {
		g_yIndex++;
		for (int i = 19; i >= 0; i--) {
			for (int j = 0; j < 10; j++) {
				if (g_backgroundArray[i][j] == 1) {
					g_backgroundArray[i + 1][j] = 1;
					g_backgroundArray[i][j] = 0;
				}
			}
		}
		InvalidateRect(hwnd, NULL, FALSE);
	}
}
BOOL Check_Buttom() {
	int flag = 1;
	for (int i = 19; i >= 0; i--) {
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 1) {
				if (i == 19) {
					flag = 0;
					return flag;
				}
				else if (g_backgroundArray[i + 1][j] == 2) {
					flag = 0;
					return flag;
				}
				else {
					flag = 1;
					continue;
				}
			}
		}
	}
	return flag;
}
void StopDown() {
	for (int i = 19; i >= 0; i--) {
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 1) {
				g_backgroundArray[i][j] = 2;
			}
		}
	}
}
void LeftMove(HWND hwnd) {
	g_xIndex--;
	int flag = 0;
	for (int i = 19; i >= 0; i--) {
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 1) {
				if (j == 0) {
					flag = 1;
					return;
				}
				else if (g_backgroundArray[i][j - 1] == 1) {
					continue;
				}
			}
		}
	}
	if (flag == 0) {
		for (int i = 19; i >= 0; i--) {
			for (int j = 0; j < 10; j++) {
				if (g_backgroundArray[i][j] == 1) {
					g_backgroundArray[i][j] = 0;
					g_backgroundArray[i][j - 1] = 1;
				}
			}
		}
	}
	InvalidateRect(hwnd, NULL, FALSE);
}
void RightMove(HWND hWnd) {
	g_xIndex++;
	int flag = 0;
	for (int i = 19; i >= 0; i--) {
		for (int j = 9; j >= 0; j--) {
			if (g_backgroundArray[i][j] == 1) {
				if (j == 9) {
					flag = 1;
					return;
				}
				else if (g_backgroundArray[i][j + 1] == 1) {
					continue;
				}
			}
		}
	}
	if (flag == 0) {
		for (int i = 19; i >= 0; i--) {
			for (int j = 9; j >= 0; j--) {
				if (g_backgroundArray[i][j] == 1) {
					g_backgroundArray[i][j] = 0;
					g_backgroundArray[i][j + 1] = 1;
				}
			}
		}
	}
	InvalidateRect(hWnd, NULL, FALSE);
}
void DownMove(HWND hwnd) {
	if (!Check_Buttom())
	{
		//停止下落
		StopDown();
		InvalidateRect(hwnd, NULL, FALSE);
		//检查是否有行消除
		Check_If_Delete_Row(hwnd);
		//产生新的方块
		OnCreate();
		InvalidateRect(hwnd, NULL, FALSE);
	}
	else {
		g_yIndex++;
		int flag = 0;
		for (int i = 19; i >= 0; i--) {
			for (int j = 0; j < 10; j++) {
				if (g_backgroundArray[i][j] == 1) {
					if (i == 19) {
						flag = 1;
						return;
					}
					else if (g_backgroundArray[i + 1][j] == 1) {
						continue;
					}
				}
			}
		}
		if (flag == 0) {
			for (int i = 19; i >= 0; i--) {
				for (int j = 0; j < 10; j++) {
					if (g_backgroundArray[i][j] == 1) {
						g_backgroundArray[i][j] = 0;
						g_backgroundArray[i + 1][j] = 1;
					}
				}
			}
		}
		InvalidateRect(hwnd, NULL, FALSE);
	}
}
void Check_If_Delete_Row(HWND hWnd) {
	int count = 0;//记录消除的行数
	for (int i = 19; i >= 0; i--) {
		int flag = 1;
		//判断该行是否全是2
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 0) {
				flag = 0;
				break;
			}
		}
		//如果该行全是2,则消除该行
		if (flag == 1) {
			count++;
			for (int j = 0; j < 10; j++) {
				g_backgroundArray[i][j] = 0;
			}
			for (int k = i; k > 0; k--) {
				for (int j = 0; j < 10; j++) {
					g_backgroundArray[k][j] = g_backgroundArray[k - 1][j];
				}
			}
		}
	}
	if (count > 0) {
		InvalidateRect(hWnd,NULL, FALSE);
	}
}
void onChangeProc(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	switch (g_blockIndex)
	{
		//0-4为一类
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		if (canSquareChange())
		{
			change0To4SquareShape(hWnd);
		}
		else
		{
			return;
		}
		break;
	case 5://5为正方形，不需要变形
		break;
	case 6://6为长条
		if (canLongSquareChange())
			changeLongSquareShape(hWnd);
		break;
	}
	ReleaseDC(hWnd, hdc);
}
int canSquareChange()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (g_backgroundArray[g_yIndex + i][g_xIndex + j] == 2)
				return 0;
		}
	}

	//第一种方案，到达边界不让其变形
	if (g_xIndex < 0 || g_xIndex + 2 >9)
	{
		return 0;
	}

	////第二种方案，到达边界仍让其变形，但g_xIndex移动一位，使其在边界上
	//if (g_xIndex < 0)
	//{
	//	g_xIndex = 0;
	//}
	//else if (g_xIndex + 2 > 9)
	//{
	//	g_xIndex = 7;
	//}
	return 1;
}
int canLongSquareChange()
{
	int i = 1, j = 1;

	for (; i < 4;)//判断中心右边距离边界或者值为2的方块有多远
	{
		if (g_backgroundArray[g_yIndex][g_xIndex + i] == 2 ||
			g_xIndex + i > 9)
		{
			i--;
			break;
		}
		i++;
	}

	for (; j < 4;)//判断中心左边距离边界或者值为2的方块有多远
	{
		if (g_backgroundArray[g_yIndex][g_xIndex - j] == 2 ||
			g_xIndex - j < 0)
		{
			j--;//因为上一轮先加了1，这轮再做判断的，所以不符合条件要减去1
			break;
		}
		j++;
	}

	if (i + j < 3)
	{
		return 0;
	}

	return 1;
}
void change0To4SquareShape(HWND hWnd)
{
	char tempArray[3][3] = { 0 };
	//将3*3数组从背景中提出来
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempArray[i][j] = g_backgroundArray[g_yIndex + i][g_xIndex + j];
		}
	}

	//将变换后的再赋值回去
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			g_backgroundArray[g_yIndex + i][g_xIndex + j] = tempArray[2 - j][i];
		}
	}
	InvalidateRect(hWnd, NULL, FALSE);
}
void changeLongSquareShape(HWND hWnd)
{
	//横着的情况
	if (g_backgroundArray[g_yIndex][g_xIndex - 1] == 1)//中心的左边为1，说明横着
	{
		//先将原背景里的清除
		g_backgroundArray[g_yIndex][g_xIndex - 1] = 0;
		g_backgroundArray[g_yIndex][g_xIndex + 0] = 0;//中心
		g_backgroundArray[g_yIndex][g_xIndex + 1] = 0;
		g_backgroundArray[g_yIndex][g_xIndex + 2] = 0;

		// 1 1 1 1 
		// 2 2 2 2 
		if (g_backgroundArray[g_yIndex + 1][g_xIndex] == 2)
		{
			//防止
			// 2 2 2 2		2 2 2 2
			// 1 1 1 1	或	0 0 0 0
			// 2 2 2 2		1 1 1 1
			//				2 2 2 2
			if (g_backgroundArray[g_yIndex - 3][g_xIndex] != 2 &&
				g_backgroundArray[g_yIndex - 2][g_xIndex] != 2 &&
				g_backgroundArray[g_yIndex - 1][g_xIndex] != 2)
			{
				//然后填上变形后的
				g_backgroundArray[g_yIndex - 3][g_xIndex] = 1;
				g_backgroundArray[g_yIndex - 2][g_xIndex] = 1;
				g_backgroundArray[g_yIndex - 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 0][g_xIndex] = 1;//中心
			}


		}
		else if (g_backgroundArray[g_yIndex + 2][g_xIndex] == 2)
		{
			// 2 2 2 2
			// 1 1 1 1
			// 0 0 0 0
			// 2 2 2 2	
			if (g_backgroundArray[g_yIndex - 1][g_xIndex] != 2 &&
				g_backgroundArray[g_yIndex - 2][g_xIndex] != 2)
			{
				//然后填上变形后的
				g_backgroundArray[g_yIndex - 2][g_xIndex] = 1;
				g_backgroundArray[g_yIndex - 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 0][g_xIndex] = 1;//中心
				g_backgroundArray[g_yIndex + 1][g_xIndex] = 1;
			}

		}
		else
		{
			//防止
			//2 2 2 2
			//1 1 1 1
			//0 0 0 0
			//0 0 0 0
			if (g_backgroundArray[g_yIndex - 1][g_xIndex] != 2)
			{
				//然后填上变形后的
				g_backgroundArray[g_yIndex - 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 0][g_xIndex] = 1;//中心
				g_backgroundArray[g_yIndex + 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 2][g_xIndex] = 1;
			}

		}

	}
	else//竖着的
	{
		//先将原背景里的清除
		g_backgroundArray[g_yIndex - 1][g_xIndex] = 0;
		g_backgroundArray[g_yIndex + 0][g_xIndex] = 0;//中心
		g_backgroundArray[g_yIndex + 1][g_xIndex] = 0;
		g_backgroundArray[g_yIndex + 2][g_xIndex] = 0;

		if (g_backgroundArray[g_yIndex][g_xIndex + 1] == 2 || g_xIndex == 9)
		{	//整体向左平移2位
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 2] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 3] = 1;

			//标记中心改变
			g_xIndex -= 2;
		}
		else if (g_backgroundArray[g_yIndex][g_xIndex + 2] == 2 || g_xIndex == 8)
		{	//整体向左平移1位
			g_backgroundArray[g_yIndex][g_xIndex + 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 2] = 1;

			//标记中心改变
			g_xIndex -= 1;
		}
		else if (g_backgroundArray[g_yIndex][g_xIndex - 1] == 2 || g_xIndex == 0)
		{	//整体向右平移1位
			g_backgroundArray[g_yIndex][g_xIndex + 3] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 2] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;

			//标记中心改变
			g_xIndex += 1;
		}
		else
		{
			//然后填上变形后的
			g_backgroundArray[g_yIndex][g_xIndex - 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;//中心
			g_backgroundArray[g_yIndex][g_xIndex + 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 2] = 1;
		}
	}
	InvalidateRect(hWnd, NULL, FALSE);
}
int IsGameOver()
{
	for (int i = 0; i < 10; i++)
	{
		if (g_backgroundArray[0][i] == 2)
		{
			return 1;
		}
	}
	return 0;
}
void OnPaintText(HDC hdc,PAINTSTRUCT* ps) {
	ps->rcPaint.left = 302;
	ps->rcPaint.top = 50;
	ps->rcPaint.right = 500;
	ps->rcPaint.bottom = 400;
	DrawText(hdc, "Welcom to Tetris\nPress enter to start\n\n\nPress space to pause\nPress up to rotate\nPress down to move down\nPress left and right to move sideways\n"
		,-1, &ps->rcPaint, 
		DT_LEFT );

}