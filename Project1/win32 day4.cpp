#include <windows.h>
#include <stdio.h>
#include<time.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 640
#define ID_DOWN_TIMER 1
#define ID_CHECK_TIMER 2

struct Block{
	int space[4][4];
}block[7][4];

int g_xIndex = 3;//��¼��������ڿռ�����Ͻ�x
int g_yIndex = 0;//��¼��������ڿռ�����Ͻ�y
int g_blockIndex = 0;//��¼��ǰ�Ķ���˹����
int g_RussBlocks[2][4] = { 0 };
int g_backgroundArray[20][10] = {0};

void StopDown();
void BlockDown();
void onChangeProc(HWND hWnd);
int canSquareChange();//�ж��Ƿ���Ըı䷽��
int canLongSquareChange();//�ж��Ƿ���Ըı䳤���η���
void OnPaint(HDC hdc);//���Ʊ���
void DrawBlock(HDC hdc);//���ƶ���˹����
void OnCreate();//������ɶ���˹����
void CopyBlock();//���ƶ���˹���鵽����
void OnTimer(HWND hWnd);//��ʱ��
void LeftMove();//���ƶ���˹����
void RightMove();//���ƶ���˹����
void DownMove();//���ƶ���˹����
void change0To4SquareShape();//�ı�0����Ϊ4����
void changeLongSquareShape();//�ı䳤���η���
int IsGameOver();//�ж���Ϸ�Ƿ����
void Check_If_Delete_Row(HWND hWnd);
BOOL Check_Buttom();//�ж��Ƿ񵽴�ײ�

//0��ʾ�հף�1��ʾ����, 2��ʾ����ײ�

//��Ϣ������
//���        ��ϢID          ��������     ��������
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
			LeftMove();
			break;
		case VK_RIGHT:
			RightMove();
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			DownMove();
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
	case WM_CREATE:       //�����һ�δ�������ʱ����
		OnCreate();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);//Ĭ�ϴ��ڴ�����,��������Ϣ�������ṩĬ�ϵĴ���ʽ
}
//������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MyWindowClass";
	wc.lpfnWndProc = WndProc;  //����Ĵ��ڴ�����������
	wc.hInstance = hInstance;

	RegisterClassEx(&wc);//��������ע�ᵽϵͳ

	//��������
	HWND hWnd = CreateWindowEx(0, "MyWindowClass", "MyWindow", WS_OVERLAPPEDWINDOW, 200, 200, 500, 640, NULL, NULL, hInstance, NULL);
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

void OnPaint(HDC hdc) {
	
	//˫������
	HDC hdcMem = CreateCompatibleDC(hdc);
	//��������λͼ
	HBITMAP hBackmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdcMem, hBackmap);
	//���ڴ�dc�ϻ���
	DrawBlock(hdcMem);
	//���Ƶ�����dc
	BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);
	//�ͷ�dc
	DeleteObject(hBackmap);
}
void DrawBlock(HDC hdc) {
	Rectangle(hdc, 0,0,300,600);
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
}
void OnCreate() {
	srand(time(NULL));
	int shape = rand() % 7;
	g_blockIndex = shape;
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
	if (IsGameOver()) {
		KillTimer(hWnd, ID_DOWN_TIMER);
		KillTimer(hWnd, ID_CHECK_TIMER);
		MessageBox(hWnd, "Game Over!", "Game Over", MB_OK);
		SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		return;
	}
	if (!Check_Buttom()) 
	{
		//ֹͣ����
		StopDown();
		//����Ƿ���������
		Check_If_Delete_Row(hWnd);
		//�����µķ���
		OnCreate();
	}
	else 
	{
		BlockDown();
	}
	InvalidateRect(hWnd, NULL, FALSE);//�ػ洰��,��ʱ����WM_PAINT��Ϣ,����OnPaint����
}
void BlockDown() {
	g_yIndex++;
	for (int i = 19; i >= 0; i--) {
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 1) {
				g_backgroundArray[i+1][j] = 1;
				g_backgroundArray[i][j] = 0;
			}
		}
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
void LeftMove() {
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
}
void RightMove() {
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
}
void DownMove() {
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
}
void Check_If_Delete_Row(HWND hWnd) {
	int count = 0;//��¼����������
	for (int i = 19; i >= 0; i--) {
		int flag = 1;
		//�жϸ����Ƿ�ȫ��2
		for (int j = 0; j < 10; j++) {
			if (g_backgroundArray[i][j] == 0) {
				flag = 0;
				break;
			}
		}
		//�������ȫ��2,����������
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
		InvalidateRect(hWnd, NULL, FALSE);
	}
}
void onChangeProc(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	switch (g_blockIndex)
	{
		//0-4Ϊһ��
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		if (canSquareChange())
		{
			change0To4SquareShape();
		}
		else
		{
			return;
		}
		break;
	case 5://5Ϊ�����Σ�����Ҫ����
		break;
	case 6://6Ϊ����
		if (canLongSquareChange())
			changeLongSquareShape();
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

	//��һ�ַ���������߽粻�������
	if (g_xIndex < 0 || g_xIndex + 2 >9)
	{
		return 0;
	}

	////�ڶ��ַ���������߽���������Σ���g_xIndex�ƶ�һλ��ʹ���ڱ߽���
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

	for (; i < 4;)//�ж������ұ߾���߽����ֵΪ2�ķ����ж�Զ
	{
		if (g_backgroundArray[g_yIndex][g_xIndex + i] == 2 ||
			g_xIndex + i > 9)
		{
			i--;
			break;
		}
		i++;
	}

	for (; j < 4;)//�ж�������߾���߽����ֵΪ2�ķ����ж�Զ
	{
		if (g_backgroundArray[g_yIndex][g_xIndex - j] == 2 ||
			g_xIndex - j < 0)
		{
			j--;//��Ϊ��һ���ȼ���1�����������жϵģ����Բ���������Ҫ��ȥ1
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
void change0To4SquareShape()
{
	char tempArray[3][3] = { 0 };
	//��3*3����ӱ����������
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			tempArray[i][j] = g_backgroundArray[g_yIndex + i][g_xIndex + j];
		}
	}

	//���任����ٸ�ֵ��ȥ
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			g_backgroundArray[g_yIndex + i][g_xIndex + j] = tempArray[2 - j][i];
		}
	}
}
void changeLongSquareShape()
{
	//���ŵ����
	if (g_backgroundArray[g_yIndex][g_xIndex - 1] == 1)//���ĵ����Ϊ1��˵������
	{
		//�Ƚ�ԭ����������
		g_backgroundArray[g_yIndex][g_xIndex - 1] = 0;
		g_backgroundArray[g_yIndex][g_xIndex + 0] = 0;//����
		g_backgroundArray[g_yIndex][g_xIndex + 1] = 0;
		g_backgroundArray[g_yIndex][g_xIndex + 2] = 0;

		// 1 1 1 1 
		// 2 2 2 2 
		if (g_backgroundArray[g_yIndex + 1][g_xIndex] == 2)
		{
			//��ֹ
			// 2 2 2 2		2 2 2 2
			// 1 1 1 1	��	0 0 0 0
			// 2 2 2 2		1 1 1 1
			//				2 2 2 2
			if (g_backgroundArray[g_yIndex - 3][g_xIndex] != 2 &&
				g_backgroundArray[g_yIndex - 2][g_xIndex] != 2 &&
				g_backgroundArray[g_yIndex - 1][g_xIndex] != 2)
			{
				//Ȼ�����ϱ��κ��
				g_backgroundArray[g_yIndex - 3][g_xIndex] = 1;
				g_backgroundArray[g_yIndex - 2][g_xIndex] = 1;
				g_backgroundArray[g_yIndex - 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 0][g_xIndex] = 1;//����
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
				//Ȼ�����ϱ��κ��
				g_backgroundArray[g_yIndex - 2][g_xIndex] = 1;
				g_backgroundArray[g_yIndex - 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 0][g_xIndex] = 1;//����
				g_backgroundArray[g_yIndex + 1][g_xIndex] = 1;
			}

		}
		else
		{
			//��ֹ
			//2 2 2 2
			//1 1 1 1
			//0 0 0 0
			//0 0 0 0
			if (g_backgroundArray[g_yIndex - 1][g_xIndex] != 2)
			{
				//Ȼ�����ϱ��κ��
				g_backgroundArray[g_yIndex - 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 0][g_xIndex] = 1;//����
				g_backgroundArray[g_yIndex + 1][g_xIndex] = 1;
				g_backgroundArray[g_yIndex + 2][g_xIndex] = 1;
			}

		}

	}
	else//���ŵ�
	{
		//�Ƚ�ԭ����������
		g_backgroundArray[g_yIndex - 1][g_xIndex] = 0;
		g_backgroundArray[g_yIndex + 0][g_xIndex] = 0;//����
		g_backgroundArray[g_yIndex + 1][g_xIndex] = 0;
		g_backgroundArray[g_yIndex + 2][g_xIndex] = 0;

		if (g_backgroundArray[g_yIndex][g_xIndex + 1] == 2 || g_xIndex == 9)
		{	//��������ƽ��2λ
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 2] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 3] = 1;

			//������ĸı�
			g_xIndex -= 2;
		}
		else if (g_backgroundArray[g_yIndex][g_xIndex + 2] == 2 || g_xIndex == 8)
		{	//��������ƽ��1λ
			g_backgroundArray[g_yIndex][g_xIndex + 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex - 2] = 1;

			//������ĸı�
			g_xIndex -= 1;
		}
		else if (g_backgroundArray[g_yIndex][g_xIndex - 1] == 2 || g_xIndex == 0)
		{	//��������ƽ��1λ
			g_backgroundArray[g_yIndex][g_xIndex + 3] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 2] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;

			//������ĸı�
			g_xIndex += 1;
		}
		else
		{
			//Ȼ�����ϱ��κ��
			g_backgroundArray[g_yIndex][g_xIndex - 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 0] = 1;//����
			g_backgroundArray[g_yIndex][g_xIndex + 1] = 1;
			g_backgroundArray[g_yIndex][g_xIndex + 2] = 1;
		}
	}
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