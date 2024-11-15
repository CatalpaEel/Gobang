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
int cal_row(int x, int y)
{
    int color = board[x][y];
    if (color == -1)
    {
        return 0;
    }
    int cnt = 1;
    for (int i = y + 1; i <= n; ++i)
    {
        if (board[x][i] == color)
        {
            ++cnt;
        }
    }
    for (int i = y - 1; i >= 1; --i)
    {
        if (board[x][i] == color)
        {
            ++cnt;
        }
    }
    return cnt;
}
int cal_col(int x, int y)
{
    int color = board[x][y];
    if (color == -1)
    {
        return 0;
    }
    int cnt = 1;
    for (int i = x + 1; i <= n; ++i)
    {
        if (board[i][y] == color)
        {
            ++cnt;
        }
    }
    for (int i = x - 1; i >= 1; --i)
    {
        if (board[i][y] == color)
        {
            ++cnt;
        }
    }
    return cnt;
}
int cal_diag(int x, int y)
{
    int color = board[x][y];
    if (color == -1)
    {
        return 0;
    }
    int cnt = 1;
    for (int i = 1; x + i <= n && y + i <= n; ++i)
    {
        if (board[x + i][y + i] == color)
        {
            ++cnt;
        }
    }
    for (int i = 1; x - i >= 1 && y - i >= 1; ++i)
    {
        if (board[x - i][y - i] == color)
        {
            ++cnt;
        }
    }
    return cnt;
}
int cal_back_diag(int x, int y)
{
    int color = board[x][y];
    if (color == -1)
    {
        return 0;
    }
    int cnt = 1;
    for (int i = 1; x + i <= n && y - i >= 1; ++i)
    {
        if (board[x + i][y - i] == color)
        {
            ++cnt;
        }
    }
    for (int i = 1; x - i >= 1 && y + i <= n; ++i)
    {
        if (board[x - i][y + i] == color)
        {
            ++cnt;
        }
    }
    return cnt;
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
    return x >= 1 && x <= n && y >= 1 && y <= n && board[x][y] == -1;
}
bool checkban(int x, int y)
{
    // todo
    return false;
}
bool player_go(int color)
{
    int x = 0, y = 0;
    while (!canput(x, y))
    {
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
    //1 system("clear");
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
    while (!checkwin())
    {
        ++cnt;
        bool flag;
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
}
int main()
{
    play();
}