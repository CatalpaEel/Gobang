#include <iostream>
using namespace std;
const int n = 15;
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

class BOARD
{
private:
    int board[16][16];

    bool is_in_board(int x, int y)
    {
        return x >= 1 && x <= n && y >= 1 && y <= n;
    }

public:
    BOARD()
    {
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                board[i][j] = -1;
            }
        }
    }
    void board_put(int x, int y, int color)
    {
        this->board[x][y] = color;
    }
    int get_color(int x, int y)
    {
        return board[x][y];
    }
    bool canput(int x, int y)
    {
        return is_in_board(x, y) && board[x][y] == -1;
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
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
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
                    int len = ry - ly - 1;
                    if (k == 1)
                    {
                        len = rx - lx - 1;
                    }
                    if (len >= 5)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // If it is banned, return true, which means that the black loses the game.
    bool checkban()
    {
        const Direction fx[4][2] = {{LEFT, RIGHT}, {UP, DOWN}, {LEFT_UP, RIGHT_DOWN}, {LEFT_DOWN, RIGHT_UP}};
        int cnt = 0;
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (board[i][j] != 1)
                {
                    continue;
                }
                for (int k = 0; k < 4; ++k)
                {
                    int lx = i, ly = j, rx, ry;
                    find_next(lx, ly, fx[k][0]); // to avoid repetition
                    find_adjacent(i, j, fx[k][1], &rx, &ry);
                    int len = ry - ly - 1;
                    if (k == 1)
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
            }
        }
        return cnt >= 2;
    }

    void printboard()
    {
        cout << "--------------------------" << endl;
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
};

int evaluate_element(BOARD *board, int x, int y)
{

    const Direction fx[4][2] = {{LEFT, RIGHT}, {UP, DOWN}, {LEFT_UP, RIGHT_DOWN}, {LEFT_DOWN, RIGHT_UP}};

    int color = board->get_color(x, y);
    if (color == -1)
    {
        return 0;
    }

    const int INF = 1e7;
    const int val_sheet[9] = {
        20000,
        8000,
        8000,
        6000,
        2000,
        2000,
        1000,
        500,
        500,
    };

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
        int len = ry - ly - 1;
        if (i == 1)
        {
            len = rx - lx - 1;
        }
        switch (len)
        {
        case 5:
            return color ? INF : -INF;
            break;
        case 4:
            cnt_canput = board->canput(lx, ly) + board->canput(rx, ry);
            switch (cnt_canput)
            {
            case 1:
                ++cnt_dead_four;
                break;
            case 2:
                ++cnt_open_four;
                break;
            }
            break;
        case 3:
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
            break;
        case 2:
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
            break;
        case 1:
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
            break;
        default: // >=6
            return -INF;
            break;
        }

        if (color && cnt_open_four + cnt_dead_four + cnt_open_three >= 2)
        {
            return -INF;
        }
    }
    int sum = 0;
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
    return color ? sum : -sum;
}

int evaluate(BOARD *board)
{
    int sum = 0;
    if (board->checkban())
    {
        return -1e7;
    }
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            sum += evaluate_element(board, i, j);
            if (sum <= -1e7 || sum >= 1e7)
            {
                return sum;
            }
        }
    }
    return sum;
}

int minmax(BOARD *board, int dep, int color, int &x, int &y)
{
    const int MAX_DEP = 3;
    const int INF = 1e8;
    if (dep == MAX_DEP)
    {
        return evaluate(board);
    }
    int mx = color ? -INF : INF;
    x = 0, y = 0;
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (board->canput(i, j))
            {
                if (!x)
                {
                    x = i, y = j;
                }
                board->board_put(i, j, color);
                int tx, ty;
                int val = minmax(board, dep + 1, color ^ 1, tx, ty);
                if (color && val > mx || !color && val < mx)
                {
                    mx = val;
                    x = i, y = j;
                }
                board->board_put(i, j, -1);
            }
        }
    }
    return mx;
}

bool player_go(BOARD *board, int color)
{
    int x = 0, y = 0;
    while (!board->canput(x, y))
    {
        cout << "Please input the coordinates:";
        cin >> x >> y;
    }
    board->board_put(x, y, color);
    if (color && board->checkban())
    {
        return false;
    }
    return true;
}

bool computer_go(BOARD *board, int color)
{
    int x, y;
    minmax(board, 1, color, x, y);
    board->board_put(x, y, color);
    if (color && board->checkban())
    {
        return false;
    }
    return true;
}
bool go(BOARD *board, int opt, int color)
{
    if (opt)
    {
        return player_go(board, color);
    }
    else
    {
        return computer_go(board, color);
    }
}
void play()
{
    BOARD *board = new BOARD();
    board->printboard();
    int black = 1, white = black ^ 1;
    int cnt = 0;
    bool flag;
    while (!board->checkwin())
    {
        ++cnt;
        if (cnt & 1)
        {
            flag = go(board, black, 1);
        }
        else
        {
            flag = go(board, white, 0);
        }
        board->printboard();
        if (!flag)
        {
            break;
        }
    }
    if (flag ^ (cnt & 1))
    {
        cout << "White win!" << endl;
    }
    else
    {
        cout << "Black win" << endl;
    }
}
int main()
{
    play();
}