#include <ctime>
#include <algorithm>
#include "jsoncpp/json.h"

#ifndef UNICODE
#define UNICODE
#endif

#ifdef BOTZONE
#include <iostream>
#endif

#ifndef BOTZONE
#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#endif

using namespace std;
const int n = 15;
clock_t START_TIME = 0;

enum Direction
{
    LEFT,       // 0
    RIGHT,      // 1
    UP,         // 2
    DOWN,       // 3
    LEFT_UP,    // 4
    LEFT_DOWN,  // 5
    RIGHT_UP,   // 6
    RIGHT_DOWN, // 7
};
struct POSITION
{
    int x, y;
    bool operator<(const POSITION &pos) const
    {
        if (x == pos.x)
        {
            return y < pos.y;
        }
        return x < pos.x;
    }
};
class BOARD
{
private:
    int board[n][n];

    bool is_in_board(int x, int y)
    {
        return x >= 0 && x < n && y >= 0 && y < n;
    }

    POSITION sta[2][n * n];
    int sta_top[2] = {0, 0};
    void add_stack(int x, int y, int color)
    {
        sta[color][++sta_top[color]] = (POSITION){x, y};
    }
    void pop_stack(int color)
    {
        --sta_top[color];
    }

public:
    BOARD()
    {
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                board[i][j] = -1;
            }
        }
    }
    BOARD *copy()
    {
        BOARD *tmp = new BOARD();
        for (int color = 0; color <= 1; ++color)
        {
            for (int i = 1; i <= sta_top[color]; ++i)
            {
                tmp->board_put(sta[color][i].x, sta[color][i].y, color);
            }
        }
        return tmp;
    }
    void board_remove(int x, int y)
    {
        pop_stack(board[x][y]);
        this->board[x][y] = -1;
    }
    void board_put(int x, int y, int color)
    {
        add_stack(x, y, color);
        this->board[x][y] = color;
    }
    int get_color(int x, int y)
    {
        if (!is_in_board(x, y))
        {
            return -2;
        }
        return board[x][y];
    }
    bool canput(int x, int y)
    {
        return is_in_board(x, y) && board[x][y] == -1;
    }
    POSITION *get_stack(int color)
    {
        return sta[color];
    }
    int get_stack_top(int color)
    {
        return sta_top[color];
    }
    bool full()
    {
        return sta_top[0] + sta_top[1] >= n * n;
    }

    // Go from (x,y) in direction of dir until arrive (resx,resy) which is blank or the other color.
    void find_adjacent(int x, int y, Direction dir, int *resx, int *resy)
    {
        const int fx[8][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
        int color = board[x][y];
        while (is_in_board(x, y) && board[x][y] == color)
        {
            find_next(x, y, dir);
        }
        *resx = x;
        *resy = y;
    }

    // Input (x,y) which is a blank, then it will return the number of color in the direction of dir.
    int find_jump(int x, int y, int color, Direction dir)
    {
        find_next(x, y, dir);
        int cnt = 0;
        while (is_in_board(x, y) && board[x][y] == color)
        {
            ++cnt;
            find_next(x, y, dir);
        }
        return cnt;
    }

    // change (x,y) to the next position in the direction of dir
    void find_next(int &x, int &y, Direction dir)
    {
        const int fx[8][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
        x += fx[dir][0];
        y += fx[dir][1];
    }

    bool checkwin()
    {
        const Direction fx[4][2] = {{LEFT, RIGHT}, {UP, DOWN}, {LEFT_UP, RIGHT_DOWN}, {LEFT_DOWN, RIGHT_UP}};
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (board[i][j] == -1)
                {
                    continue;
                }
                for (int k = 0; k < 4; ++k)
                {
                    int lx, ly, rx, ry;
                    find_adjacent(i, j, fx[k][0], &lx, &ly);
                    find_adjacent(i, j, fx[k][1], &rx, &ry);
                    int len = (k == 1 ? rx - lx - 1 : ry - ly - 1);
                    if (len >= 5)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool checkban(int x, int y)
    {
        const Direction fx[4][2] = {{LEFT, RIGHT}, {UP, DOWN}, {LEFT_UP, RIGHT_DOWN}, {LEFT_DOWN, RIGHT_UP}};
        int three_three = 0;
        int four_four = 0;
        for (int i = 0; i < 4; ++i)
        {
            int lx, ly, rx, ry;
            find_adjacent(x, y, fx[i][0], &lx, &ly);
            find_adjacent(x, y, fx[i][1], &rx, &ry);
            int len = (i == 1 ? rx - lx - 1 : ry - ly - 1);
            if (len >= 6)
            {
                return true; // XXXXXX
            }
            if (len == 4)
            {
                if (canput(lx, ly) && (find_next(lx, ly, fx[i][0]), get_color(lx, ly) != 1))
                {
                    ++four_four; // O-XXXX
                }
                else if (canput(rx, ry) && (find_next(rx, ry, fx[i][1]), get_color(rx, ry) != 1))
                {
                    ++four_four; // XXXX-O
                }
            }
            else if (len == 3)
            {
                int tx, ty;
                if (get_color(lx, ly) != 1 && canput(tx = rx, ty = ry) && (find_next(tx, ty, fx[i][1]), get_color(tx, ty) == 1) && (find_next(tx, ty, fx[i][1]), get_color(tx, ty) != 1))
                {
                    ++four_four; // OXXX-XO
                }
                else if (get_color(rx, ry) != 1 && canput(tx = lx, ty = ly) && (find_next(tx, ty, fx[i][0]), get_color(tx, ty) == 1) && (find_next(tx, ty, fx[i][0]), get_color(tx, ty) != 1))
                {
                    ++four_four; // OX-XXXO
                }
                if (canput(lx, ly) && canput(rx, ry))
                {
                    find_next(lx, ly, fx[i][0]);
                    find_next(rx, ry, fx[i][1]);
                    if (canput(lx, ly) && get_color(rx, ry) != 1)
                    {
                        ++three_three; // --XXX-O
                    }
                    else if (canput(rx, ry) && get_color(lx, ly) != 1)
                    {
                        ++three_three; // O-XXX--
                    }
                }
            }
            else if (len == 2)
            {
                int tx, ty;
                if (get_color(lx, ly) != 1 && canput(tx = rx, ty = ry) && (find_next(tx, ty, fx[i][1]), get_color(tx, ty) == 1) && (find_next(tx, ty, fx[i][1]), get_color(tx, ty) == 1) && (find_next(tx, ty, fx[i][1]), get_color(tx, ty) != 1))
                {
                    ++four_four; // OXX-XXO
                }
                else if (get_color(rx, ry) != 1 && canput(tx = lx, ty = ly) && (find_next(tx, ty, fx[i][0]), get_color(tx, ty) == 1) && (find_next(tx, ty, fx[i][0]), get_color(tx, ty) == 1) && (find_next(tx, ty, fx[i][0]), get_color(tx, ty) != 1))
                {
                    ++four_four; // OXX-XXO
                }
                if (canput(lx, ly) && canput(rx, ry))
                {
                    find_next(lx, ly, fx[i][0]);
                    find_next(rx, ry, fx[i][1]);
                    if (get_color(lx, ly) == 1 && (find_next(lx, ly, fx[i][0]), canput(lx, ly)))
                    {
                        ++three_three; // -X-XX-
                    }
                    else if (get_color(rx, ry) == 1 && (find_next(rx, ry, fx[i][1]), canput(rx, ry)))
                    {
                        ++three_three; // -XX-X-
                    }
                }
            }
            else if (len == 1)
            {
                if (canput(lx, ly) && canput(rx, ry))
                {
                    find_next(lx, ly, fx[i][0]);
                    find_next(rx, ry, fx[i][1]);
                    if (get_color(lx, ly) == 1 && (find_next(lx, ly, fx[i][0]), get_color(lx, ly) == 1) && (find_next(lx, ly, fx[i][0]), canput(lx, ly)))
                    {
                        ++three_three; // -XX-X-
                    }
                    else if (get_color(rx, ry == 1) && (find_next(rx, ry, fx[i][1]), get_color(rx, ry) == 1) && (find_next(rx, ry, fx[i][1]), canput(rx, ry)))
                    {
                        ++three_three; // -X-XX-
                    }
                }
            }
        }
        return three_three >= 2 || four_four >= 2;
    }
};

bool check_overtime()
{
    if (START_TIME == 0)
    {
        return false;
    }
    return 1.0 * (clock() - START_TIME) / CLOCKS_PER_SEC > 0.9;
}

int evaluate_element(BOARD *board, int x, int y)
{
    const Direction fx[4][2] = {{LEFT, RIGHT}, {UP, DOWN}, {LEFT_UP, RIGHT_DOWN}, {LEFT_DOWN, RIGHT_UP}};
    const int INF = 1e7;
    const int val_sheet[9] = {
        1000, // open four
        1000, // dead four
        800,  // jump four
        800,  // open three
        50,   // sleep three
        100,  // jump three
        20,   // open two
        5,    // sleep two
        5,    // jump two
    };

    int color = board->get_color(x, y);
    if (color == -1)
    {
        return 0;
    }

    int cnt_open_four = 0;
    int cnt_dead_four = 0;
    int cnt_jump_four = 0;
    int cnt_open_three = 0;
    int cnt_sleep_three = 0;
    int cnt_jump_three = 0;
    int cnt_open_two = 0;
    int cnt_sleep_two = 0;
    int cnt_jump_two = 0;

    for (int i = 0; i < 4; ++i)
    {
        int lx, ly, rx, ry;
        board->find_adjacent(x, y, (Direction)fx[i][0], &lx, &ly);
        board->find_adjacent(x, y, (Direction)fx[i][1], &rx, &ry);
        int cnt_canput;
        int len = (i == 1 ? rx - lx - 1 : ry - ly - 1);
        if (len >= 5)
        {
            return color ? INF : -INF;
        }
        else if (len == 4)
        {
            cnt_canput = board->canput(lx, ly) + board->canput(rx, ry);
            if (cnt_canput == 2)
            {
                ++cnt_open_four;
            }
            else if (cnt_canput == 1)
            {
                ++cnt_dead_four;
            }
        }
        else if (len == 3)
        {
            cnt_canput = board->canput(lx, ly) + board->canput(rx, ry);
            if (board->canput(lx, ly))
            {
                if (board->find_jump(lx, ly, color, (Direction)fx[i][0]) == 1)
                {
                    ++cnt_jump_four;
                }
                if (board->find_jump(rx, ry, color, (Direction)fx[i][1]) == 1)
                {
                    ++cnt_jump_four;
                }
            }
            if (cnt_canput == 2)
            {
                ++cnt_open_three;
            }
            else if (cnt_canput == 1)
            {
                ++cnt_sleep_three;
            }
        }
        else if (len == 2)
        {
            cnt_canput = board->canput(lx, ly) + board->canput(rx, ry);
            if (board->canput(lx, ly))
            {
                switch (board->find_jump(lx, ly, color, (Direction)fx[i][0]))
                {
                case 1:
                    ++cnt_jump_three;
                    break;
                case 2:
                    ++cnt_jump_four;
                    break;
                }
                switch (board->find_jump(rx, ry, color, (Direction)fx[i][1]))
                {
                case 1:
                    ++cnt_jump_three;
                    break;
                case 2:
                    ++cnt_jump_four;
                    break;
                }
            }
            if (cnt_canput == 2)
            {
                ++cnt_open_two;
            }
            else if (cnt_canput == 1)
            {
                ++cnt_sleep_two;
            }
        }
        else if (len == 1)
        {
            if (board->canput(lx, ly))
            {
                switch (board->find_jump(lx, ly, color, (Direction)fx[i][0]))
                {
                case 1:
                    ++cnt_jump_two;
                    break;
                case 2:
                    ++cnt_jump_three;
                    break;
                case 3:
                    ++cnt_jump_four;
                    break;
                }
                switch (board->find_jump(rx, ry, color, (Direction)fx[i][1]))
                {
                case 1:
                    ++cnt_jump_two;
                    break;
                case 2:
                    ++cnt_jump_three;
                    break;
                case 3:
                    ++cnt_jump_four;
                    break;
                }
            }
        }
    }

    int sum = 0;
    if (cnt_open_three + cnt_dead_four + cnt_jump_four >= 2 || cnt_open_four)
    {
        sum = 10000;
    }
    sum =
        cnt_open_four * val_sheet[0] +
        cnt_dead_four * val_sheet[1] +
        cnt_jump_four * val_sheet[2] +
        cnt_open_three * val_sheet[3] +
        cnt_sleep_three * val_sheet[4] +
        cnt_jump_three * val_sheet[5] +
        cnt_open_two * val_sheet[6] +
        cnt_sleep_two * val_sheet[7] +
        cnt_jump_two * val_sheet[8];
    sum += (x + 1) * (n - x - 1) / n + (y + 1) * (n - y - 1) / n;

    return color ? sum : -sum;
}

int evaluate(BOARD *board, int tx, int ty, int color)
{
    const float P = 1.5; // defense rate

    if (board->get_color(tx, ty) == 1 && board->checkban(tx, ty))
    {
        return -1e7;
    }

    int sum[2] = {0, 0}; // {white ,black}
    POSITION *sta;
    int top;
    for (int k = 0; k < 2; ++k)
    {
        sta = board->get_stack(k);
        top = board->get_stack_top(k);
        for (int i = 1; i <= top; ++i)
        {
            int x = sta[i].x, y = sta[i].y;
            int val = evaluate_element(board, x, y);
            if (val <= -1e7 || val >= 1e7)
            {
                return val;
            }
            sum[k] += val;
        }
    }
    sum[color ^ 1] *= P;
    return sum[0] + sum[1];
}

int minmax(BOARD *board, int dep, int color, int &x, int &y, int front)
{
    const int MAX_DEP = 6;
    const int MAX_CHILD = 20;
    const int INF = 1e8;

    if (x != -1)
    {
        if (dep >= MAX_DEP)
        {
            return evaluate(board, x, y, color ^ 1);
        }
        if (check_overtime()) // time breaker
        {
            return evaluate(board, x, y, color ^ 1);
        }
    }

    pair<int, POSITION> sta[n * n];
    int top = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (board->canput(i, j))
            {
                board->board_put(i, j, color);
                int val = evaluate(board, i, j, color) * (MAX_DEP - dep + 1);
                board->board_remove(i, j);
                if (color && val >= 1e7 || !color && val <= -1e7)
                {
                    x = i, y = j;
                    return val;
                }
                sta[++top] = make_pair(color ? -val : val, (POSITION){i, j});
            }
        }
    }
    sort(sta + 1, sta + top + 1);

    int alpha = INF, beta = -INF;
    if (color)
    {
        alpha = front;
    }
    else
    {
        beta = front;
    }

    x = -1, y = -1;
    top = min(top, MAX_CHILD);
    for (int k = 1; k <= top; ++k)
    {
        int i = sta[k].second.x, j = sta[k].second.y;
        if (x == -1)
        {
            x = i, y = j;
        }
        board->board_put(i, j, color);
        int tx, ty, val;
        if (color) // max node
        {
            tx = i, ty = j;
            val = minmax(board, dep + 1, color ^ 1, tx, ty, beta);
            if (val > beta)
            {
                beta = val;
                x = i, y = j;
            }
        }
        else // min node
        {
            tx = i, ty = j;
            val = minmax(board, dep + 1, color ^ 1, tx, ty, alpha);
            if (val < alpha)
            {
                alpha = val;
                x = i, y = j;
            }
        }
        board->board_remove(i, j);
        if (beta >= alpha)
        {
            break;
        }
    }

    return color ? beta : alpha;
}

#ifdef BOTZONE

void botzone_play()
{
    START_TIME = clock();
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    int turnID = input["responses"].size();
    BOARD *board = new BOARD();

    // rebuild the board
    int color = 0; // 0 white; 1 black
    if (turnID > 0)
    {
        int i = 0;
        if (input["requests"][i]["x"].asInt() == -1)
        {
            color = 1;
        }
        else
        {
            color = 0;
            board->board_put(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), color ^ 1);
        }
        board->board_put(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), color);
    }
    for (int i = 1; i < turnID; i++)
    {

        board->board_put(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), color ^ 1);
        board->board_put(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), color);
    }
    board->board_put(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), color ^ 1);

    // calculate the action
    Json::Value ret;

    if (!board->checkwin())
    {
        int x, y;
        minmax(board, 1, color, x = -1, y = -1, color ? 1e8 : -1e8);
        Json::Value action;
        action["x"] = x;
        action["y"] = y;
        ret["response"] = action;
    }

    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
}

#endif

#ifndef BOTZONE

struct ComputerGoINFO
{
    BOARD *board;
    int colorComputer;
    bool *isPlayerTurn;
    int *gameOver;
    HWND window;
    ComputerGoINFO(BOARD *board, int colorComputer, bool *isPlayerTurn, int *gameOver, HWND window)
    {
        this->board = board;
        this->colorComputer = colorComputer;
        this->isPlayerTurn = isPlayerTurn;
        this->gameOver = gameOver;
        this->window = window;
    }
};

DWORD WINAPI ComputerGo(LPVOID lPThreadParameter)
{
    ComputerGoINFO *pINFO = (ComputerGoINFO *)lPThreadParameter;

    int x, y;
    START_TIME = clock();
    minmax(pINFO->board->copy(), 1, pINFO->colorComputer, x = -1, y = -1, pINFO->colorComputer ? 1e8 : -1e8);
    pINFO->board->board_put(x, y, pINFO->colorComputer);

    if (pINFO->colorComputer && pINFO->board->checkban(x, y))
    {
        *pINFO->gameOver = 0; // White win
    }
    if (pINFO->board->checkwin())
    {
        *pINFO->gameOver = pINFO->colorComputer;
    }
    else if (pINFO->board->full())
    {
        *pINFO->gameOver = 2;
    }

    *pINFO->isPlayerTurn = true;
    InvalidateRect(pINFO->window, NULL, FALSE);
    return 0;
}

template <class DERIVED_TYPE>
class BaseWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE *pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
            pThis = (DERIVED_TYPE *)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL) {}

    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0)
    {
        WNDCLASS wc = {0};

        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle,
            ClassName(),
            lpWindowName,
            dwStyle,
            x, y, nWidth, nHeight,
            hWndParent,
            hMenu,
            GetModuleHandle(NULL),
            this);

        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; }

protected:
    virtual PCWSTR ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hwnd;
};

template <class T>
void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class MainWindow : public BaseWindow<MainWindow>
{
    ID2D1Factory *pFactory;
    ID2D1HwndRenderTarget *pRenderTarget;
    ID2D1SolidColorBrush *pBrush;
    float gridStartX, gridStartY, gridEndX, gridEndY, gridGap;
    int placedX, placedY;
    BOARD *board;
    int colorPlayer, colorComputer;
    bool isPlayerTurn;
    int gameOver;

    void CalculateLayout();
    HRESULT CreateGraphicsResources();
    void DiscardGraphicsResources();
    void OnPaint();
    void Resize();
    void OnMouseMove(LPARAM lParam);
    void OnLButtonUp(LPARAM lParam);
    bool ScreenToBoardPosition(int &x, int &y);
    bool BoardToScreenPosition(int &x, int &y);
    void ComputerGoCoroutine();

public:
    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
    {
        board = new BOARD();
        colorPlayer = 1;
        colorComputer = 0;
        gameOver = -1;
        if (colorPlayer)
        {
            isPlayerTurn = true;
        }
        else
        {
            isPlayerTurn = false;
            ComputerGoCoroutine();
        }
    }

    PCWSTR ClassName() const { return L"Renju Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

void MainWindow::ComputerGoCoroutine()
{
    ComputerGoINFO *pINFO = new ComputerGoINFO(board, colorComputer, &isPlayerTurn, &gameOver, Window());
    HANDLE hThread = CreateThread(NULL, 0, ComputerGo, pINFO, 0, 0);
    CloseHandle(hThread);
}

bool MainWindow::ScreenToBoardPosition(int &x, int &y)
{
    x = 1.0 * (x - gridStartX) / gridGap + 0.5;
    y = 1.0 * (y - gridStartY) / gridGap + 0.5;
    if (x < 0 || x >= n || y < 0 || y >= n)
    {
        return false;
    }
    return true;
}

bool MainWindow::BoardToScreenPosition(int &x, int &y)
{
    if (x < 0 || x >= n || y < 0 || y >= n)
    {
        return false;
    }
    x = gridStartX + x * gridGap;
    y = gridStartY + y * gridGap;
    return true;
}

void MainWindow::OnLButtonUp(LPARAM lParam) // PlayerGo
{
    if (!isPlayerTurn || gameOver != -1)
    {
        return;
    }

    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    ScreenToBoardPosition(x, y);
    placedX = x;
    placedY = y;

    if (board->canput(x, y))
    {
        board->board_put(x, y, 1);
        InvalidateRect(Window(), NULL, FALSE);
        isPlayerTurn = false;

        if (colorPlayer && board->checkban(x, y))
        {
            gameOver = 0; // White win
        }
        if (board->checkwin())
        {
            gameOver = colorPlayer;
        }
        else if (board->full())
        {
            gameOver = 2;
        }

        if (gameOver == -1)
        {
            ComputerGoCoroutine();
        }
    }
}

void MainWindow::OnMouseMove(LPARAM lParam)
{
    if (!isPlayerTurn || gameOver != -1)
    {
        return;
    }
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    ScreenToBoardPosition(x, y);

    if (placedX != x || placedY != y)
    {
        InvalidateRect(Window(), NULL, FALSE);
    }
    placedX = x;
    placedY = y;
}

// Recalculate drawing layout when the size of the window changes.
void MainWindow::CalculateLayout()
{
    if (pRenderTarget != NULL)
    {
        D2D1_SIZE_F size = pRenderTarget->GetSize();
        const float cx = size.width / 2;
        const float cy = size.height / 2;
        const float x = size.width / 2.2;
        const float y = size.height / 2.2;
        const float a = x < y ? x : y;
        gridStartX = cx - a;
        gridStartY = cy - a;
        gridEndX = cx + a;
        gridEndY = cy + a;
        gridGap = a * 2 / (n - 1);
    }
}

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

            if (SUCCEEDED(hr))
            {
                CalculateLayout();
            }
        }
    }
    return hr;
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}

void MainWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        pRenderTarget->BeginDraw();

        pRenderTarget->Clear(D2D1::ColorF(0xF8B77F)); // Background color

        // Draw chess board
        pBrush->SetColor(D2D1::ColorF(0, 0, 0));
        for (int i = 0; i < n; ++i)
        {
            pRenderTarget->DrawLine(D2D1::Point2F(gridStartX + i * gridGap, gridStartY), D2D1::Point2F(gridStartX + i * gridGap, gridEndY), pBrush);
            pRenderTarget->DrawLine(D2D1::Point2F(gridStartX, gridStartY + i * gridGap), D2D1::Point2F(gridEndX, gridStartY + i * gridGap), pBrush);
        }

        // Draw chess
        int x, y;
        for (int color = 0; color <= 1; ++color)
        {
            if (color)
            {
                pBrush->SetColor(D2D1::ColorF(0, 0, 0));
            }
            else
            {
                pBrush->SetColor(D2D1::ColorF(1, 1, 1));
            }

            POSITION *sta = board->get_stack(color);
            int top = board->get_stack_top(color);
            for (int i = 1; i <= top; ++i)
            {
                x = sta[i].x;
                y = sta[i].y;
                BoardToScreenPosition(x, y);
                pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), gridGap * 0.4, gridGap * 0.4), pBrush);
            }
        }

        // Stress the last chess

        pBrush->SetColor(D2D1::ColorF(0, 1, 0));
        POSITION *sta = board->get_stack(isPlayerTurn ^ colorPlayer);
        int top = board->get_stack_top(isPlayerTurn ^ colorPlayer);
        if (top)
        {
            x = sta[top].x;
            y = sta[top].y;
            BoardToScreenPosition(x, y);
            pRenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), gridGap * 0.41, gridGap * 0.41), pBrush);
        }

        // Draw next predicted chess
        if (isPlayerTurn && gameOver == -1)
        {
            x = placedX, y = placedY;
            printf("%d %d\n", x, y);
            if (board->canput(x, y) && BoardToScreenPosition(x, y))
            {
                pBrush->SetColor(D2D1::ColorF(0, 0, 0, 0.5));
                pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), gridGap * 0.4, gridGap * 0.4), pBrush);
            }
        }

        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);
        CalculateLayout();
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1; // Fail CreateWindowEx.
        }
        return 0;

    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_SIZE:
        Resize();
        return 0;

    case WM_MOUSEMOVE:
        OnMouseMove(lParam);
        return 0;

    case WM_LBUTTONUP:
        OnLButtonUp(lParam);
        return 0;
    }

    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow)
{
    MainWindow win;

    if (!win.Create(L"Renju-Official", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);
    UpdateWindow(win.Window());

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

#endif