#include <Windows.h>
#include <tchar.h>
#include <time.h>

#define COL 10
#define ROW 20
#define SIZE 30
#define BACKGROUND 0x00000000

enum class BlockTypes
{
    single, mountain, L, hook, bar, field
};

class Blocks
{
public:
    POINT* pos;
    int len;
    COLORREF col;

    Blocks() : pos(NULL), len(0), col(0) {}

    Blocks(BlockTypes type)
    {
        pos = NULL;
        len = 0;
        col = RGB(rand() % 230 + 26, rand() % 230 + 26, rand() % 230 + 26);

        switch (type)
        {
        case BlockTypes::single:
            pos = new POINT[1];
            len = 1;
            pos[0] = { 0, 0 };
            break;
        case BlockTypes::mountain:
            pos = new POINT[4];
            len = 4;
            pos[0] = { 0, 0 };
            pos[1] = { -1, 0 };
            pos[2] = { 1, 0 };
            pos[3] = { 0, -1 };
            break;
        case BlockTypes::L:
            pos = new POINT[4];
            len = 4;
            pos[0] = { 0, 0 };
            pos[1] = { 0, -1 };
            pos[2] = { 0, -2 };
            pos[3] = { 1, 0 };
            break;
        case BlockTypes::hook:
            pos = new POINT[4];
            len = 4;
            pos[0] = { 0, 0 };
            pos[1] = { 0, -1 };
            pos[2] = { 0, -2 };
            pos[3] = { -1, 0 };
            break;
        case BlockTypes::bar:
            pos = new POINT[4];
            len = 4;
            pos[0] = { 0, 0 };
            pos[1] = { 0, 1 };
            pos[2] = { 0, -1 };
            pos[3] = { 0, -2 };
            break;
        case BlockTypes::field:
            pos = new POINT[4];
            len = 4;
            pos[0] = { 0, 0 };
            pos[1] = { 1, 0 };
            pos[2] = { 0, -1 };
            pos[3] = { 1, -1 };
            break;
        default:
            break;
        }
    }

    Blocks(const Blocks& block)
    {
        this->pos = new POINT[block.len];
        this->len = block.len;
        this->col = block.col;
        for (int i = 0; i < block.len; i++)
        {
            this->pos[i] = block.pos[i];
        }
    }

    void operator=(Blocks block)
    {
        this->pos = new POINT[block.len];
        this->len = block.len;
        this->col = block.col;
        for (int i = 0; i < block.len; i++)
        {
            this->pos[i] = block.pos[i];
        }
    }

    ~Blocks()
    {
        if (pos != NULL)
            delete[] pos;
    }
};

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2)
{
    HGDIOBJ old = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x1, y1, x2, y2);
    DeleteObject(SelectObject(hdc, old));
}

void DrawSolidRectangle(HDC hdc, int x1, int y1, int x2, int y2)
{
    HGDIOBJ old = SelectObject(hdc, CreatePen(PS_NULL, 0, 0));
    Rectangle(hdc, x1, y1, x2, y2);
    DeleteObject(SelectObject(hdc, old));
}

void DrawFillRectangle(HDC hdc, int x1, int y1, int x2, int y2)
{
    Rectangle(hdc, x1, y1, x2, y2);
}

COLORREF map[ROW][COL] = { BACKGROUND };
bool AuxiliaryMap[ROW][COL] = { false };
Blocks nowBlo, nextBlo;
bool isSpace = false;

void DrawMap(int beginX, int beginY, int size, COLORREF map[ROW][COL], HDC hdc)
{
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, BACKGROUND)));
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            DeleteObject(SelectObject(hdc, CreateSolidBrush(map[i][j])));
            DrawFillRectangle(hdc, beginX + j * size, beginY + i * size, beginX + (j + 1) * size, beginY + (i + 1) * size);
        }
    }
}

void DrawNextFrame(int beginX, int beginY, int size, HDC hdc)
{
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, RGB(255, 255, 255))));
    DeleteObject(SelectObject(hdc, CreateSolidBrush(RGB(128, 128, 128))));
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            DrawFillRectangle(hdc, beginX + j * size, beginY + i * size, beginX + (j + 1) * size, beginY + (i + 1) * size);
        }
    }
}

void DrawNowBlock(int beginX, int beginY, int size, const Blocks& block, HDC hdc)
{
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, BACKGROUND)));
    DeleteObject(SelectObject(hdc, CreateSolidBrush(block.col)));
    for (int i = 0; i < block.len; i++)
    {
        int xx = block.pos[i].x, yy = block.pos[i].y;
        DrawFillRectangle(hdc, beginX + xx * size, beginY + yy * size, beginX + (xx + 1) * size, beginY + (yy + 1) * size);
    }
}

void DrawNextBlock(int beginX, int beginY, int size, const Blocks& block, HDC hdc)
{
    DeleteObject(SelectObject(hdc, CreatePen(PS_SOLID, 0, BACKGROUND)));
    DeleteObject(SelectObject(hdc, CreateSolidBrush(block.col)));
    for (int i = 0; i < block.len; i++)
    {
        int xx = block.pos[i].x, yy = block.pos[i].y;
        DrawFillRectangle(hdc, beginX + xx * size, beginY + yy * size, beginX + (xx + 1) * size, beginY + (yy + 1) * size);
    }
}

void PutNowBlockInHead(Blocks& nowBlo, int HeadX, int HeadY)
{
    for (int i = 0; i < nowBlo.len; i++)
    {
        nowBlo.pos[i].x += HeadX;
        nowBlo.pos[i].y += HeadY;
    }
}

BlockTypes GetRandBlock()
{
    return BlockTypes(rand() % 6);
}

bool NowBlockDown(Blocks& nowBlo, bool AuxiliaryMap[ROW][COL])
{
    bool result = true;
    for (int i = 0; i < nowBlo.len; i++)
    {
        int xx = nowBlo.pos[i].x, yy = nowBlo.pos[i].y;
        if (xx >= 0 && xx < COL && yy < ROW - 1)
        {
            if (yy + 1 >= 0)
            {
                if (AuxiliaryMap[yy + 1][xx] == false);
                else result = false;
            }
        }
        else result = false;
    }
    if (result) for (int i = 0; i < nowBlo.len; i++) nowBlo.pos[i].y++;
    return result;
}

bool NowBlockLeft(Blocks& block, bool AuxiliaryMap[ROW][COL])
{
    bool result = true;
    for (int i = 0; i < block.len; i++)
    {
        int xx = block.pos[i].x, yy = block.pos[i].y;
        if (yy >= 0 && yy < ROW && xx > 0)
        {
            if (xx - 1 >= 0 && xx - 1 < COL)
            {
                if (AuxiliaryMap[yy][xx - 1] == false);
                else result = false;
            }
        }
        else result = false;
    }
    if (result) for (int i = 0; i < block.len; i++) block.pos[i].x--;
    return result;
}

bool NowBlockRight(Blocks& block, bool AuxiliaryMap) {

}