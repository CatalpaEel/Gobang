#include <iostream>
#include <random>
using namespace std;
int n = 15;
int board[20][20];

void initboard()
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            board[i][j] = -1;
        }
    }
}

bool is_in_board(int x, int y)
{
    return x >= 1 && x <= n && y >= 1 && y <= n;
}

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

//Go from (x,y) in direction of dir until arrive (resx,resy) which is blank or the other color.
void find_adjacent(int x, int y, Direction dir, int *resx, int *resy)
{
    const int fx[8][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    int color = board[x][y];
    while (is_in_board(x, y) && board[x][y] == color)
    {
        x += fx[dir][0];
        y += fx[dir][1];
    }
    *resx = x;
    *resy = y;
}
int cal_row(int x, int y)
{
    if (board[x][y] == -1)
    {
        return 0;
    }
    int lx, ly, rx, ry;
    find_adjacent(x, y, LEFT, &lx, &ly);
    find_adjacent(x, y, RIGHT, &rx, &ry);
    return ry - ly - 1;
}
int cal_col(int x, int y)
{
    if (board[x][y] == -1)
    {
        return 0;
    }
    int lx, ly, rx, ry;
    find_adjacent(x, y, UP, &lx, &ly);
    find_adjacent(x, y, DOWN, &rx, &ry);
    return rx - lx - 1;
}
int cal_diag(int x, int y)
{
    if (board[x][y] == -1)
    {
        return 0;
    }
    int lx, ly, rx, ry;
    find_adjacent(x, y, LEFT_UP, &lx, &ly);
    find_adjacent(x, y, RIGHT_DOWN, &rx, &ry);
    return ry - ly - 1;
}
int cal_back_diag(int x, int y)
{
    if (board[x][y] == -1)
    {
        return 0;
    }
    int lx, ly, rx, ry;
    find_adjacent(x, y, LEFT_DOWN, &lx, &ly);
    find_adjacent(x, y, RIGHT_UP, &rx, &ry);
    return ry - ly - 1;
}

bool checkwin()
{
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (cal_row(i, j) >= 5)
            {
                return true;
            }
            if (cal_col(i, j) >= 5)
            {
                return true;
            }
            if (cal_diag(i, j) >= 5)
            {
                return true;
            }
            if (cal_back_diag(i, j) >= 5)
            {
                return true;
            }
        }
    }
    return false;
}

bool canput(int x, int y)
{
    return is_in_board(x, y) && board[x][y] == -1;
}

bool checkban(int x, int y)
{
    const Direction fx[4][2] = {{LEFT, RIGHT}, {UP, DOWN}, {LEFT_UP, RIGHT_DOWN}, {LEFT_DOWN, RIGHT_UP}};
    int cnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        int lx, ly, rx, ry;
        find_adjacent(x, y, fx[i][0], &lx, &ly);
        find_adjacent(x, y, fx[i][1], &rx, &ry);
        int len = ry - ly - 1;
        if (i == 1)
        {
            len = rx - lx - 1;
        }
        if (len >= 6)
        {
            return true;
        }
        else if (len == 4 && (canput(lx, ly) || canput(rx, ry)))
        {
            ++cnt;
        }
        else if (len == 3 && canput(lx, ly) && canput(rx, ry))
        {
            ++cnt;
        }
    }
    if (cnt >= 2)
    {
        return true;
    }
    return false;
}
bool player_go(int color)
{
    int x = 0, y = 0;
    while (!canput(x, y))
    {
        cout << "Please input the coordinates:";
        cin >> x >> y;
    }
    board[x][y] = color;
    if (color)
    {
        if (checkban(x, y))
        {
            return false;
        }
    }
    return true;
}
bool computer_go(int color)
{
    // todo
    return player_go(color);
}
bool go(int opt, int color)
{
    if (opt)
    {
        return player_go(color);
    }
    else
    {
        return computer_go(color);
    }
}
void printboard()
{
    // 1 system("clear");
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            switch (board[i][j])
            {
            case 0:
                cout << "|○|";
                break;
            case 1:
                cout << "|●|";
                break;
            default:
                cout << "|_|";
                break;
            }
        }
        cout << endl;
    }
}
void play()
{
    initboard();
    printboard();
    int black = rand() & 1, white = black ^ 1;
    int cnt = 0;
    bool flag;
    while (!checkwin())
    {
        ++cnt;
        if (cnt & 1)
        {
            flag = go(black, 1);
        }
        else
        {
            flag = go(white, 0);
        }
        printboard();
        if (!flag)
        {
            break;
        }
    }
    if (flag ^ (cnt & 1))
        cout << "White win!" << endl;
    else
        cout << "Black win" << endl;
}
int main()
{
    play();
}