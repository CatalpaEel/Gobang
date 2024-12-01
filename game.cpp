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
struct POSITION
{
    int x, y;
};
class BOARD
{
private:
    int board[n + 1][n + 1];

    bool is_in_board(int x, int y)
    {
        return x >= 1 && x <= n && y >= 1 && y <= n;
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
        if (color != -1)
        {
            add_stack(x, y, color);
        }
        else
        {
            pop_stack(board[x][y]);
        }
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
    POSITION *get_stack(int color)
    {
        return sta[color];
    }
    int get_stack_top(int color)
    {
        return sta_top[color];
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
                    if (is_in_board(lx, ly) && board[lx][ly] == 1)
                    {
                        continue;
                    }
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
                    cout << "|X|"; // White
                    break;
                case 1:
                    cout << "|O|"; // Black
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
        500, // open four
        100, // dead four
        80,  // jump four
        80,  // open three
        20,  // sleep three
        20,  // jump three
        10,  // open two
        5,   // sleep two
        5,   // jump two
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

    sum += x * (n - x) / n + y * (n - y) / n;

    return color ? sum : -sum;
}

int evaluate(BOARD *board)
{
    const float P = 1.5;
    int sum[2] = {0, 0}; // {white ,black}
    POSITION *sta;
    int top;
    for (int color = 0; color < 2; ++color)
    {
        sta = board->get_stack(color);
        top = board->get_stack_top(color);
        for (int i = 1; i <= top; ++i)
        {
            int x = sta[i].x, y = sta[i].y;
            int val = evaluate_element(board, x, y);
            if (val <= -1e7 || val >= 1e7)
            {
                return val;
            }
            sum[color] += val;
        }
    }
    return sum[1] + sum[0];
    // int sum = 0;
    // if (board->checkban())
    // {
    //     return -1e7;
    // }
    // for (int i = 1; i <= n; ++i)
    // {
    //     for (int j = 1; j <= n; ++j)
    //     {
    //         int t = evaluate_element(board, i, j);
    //         if (t <= -1e7 || t >= 1e7)
    //         {
    //             return t;
    //         }
    //         sum += t;
    //     }
    // }
    // return sum;
}

int minmax(BOARD *board, int dep, int color, int &x, int &y, int front)
{
    const int MAX_DEP = 4;
    const int INF = 1e8;
    if (dep == MAX_DEP)
    {
        return evaluate(board);
    }
    int tmp = evaluate(board);
    if (color && tmp <= -1e7 || !color && tmp >= 1e7)
    {
        return tmp * (MAX_DEP - dep + 1);
    }
    int alpha = INF, beta = -INF;
    if (color)
    {
        alpha = front;
    }
    else
    {
        beta = front;
    }
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
                int tx, ty, val;
                if (color) // max node
                {
                    val = minmax(board, dep + 1, color ^ 1, tx, ty, beta);
                    if (val > beta)
                    {
                        beta = val;
                        x = i, y = j;
                    }
                }
                else // min node
                {
                    val = minmax(board, dep + 1, color ^ 1, tx, ty, alpha);
                    if (val < alpha)
                    {
                        alpha = val;
                        x = i, y = j;
                    }
                }
                board->board_put(i, j, -1);
                if (beta >= alpha)
                {
                    break;
                }
            }
        }
        if (beta >= alpha)
        {
            break;
        }
    }
    return color ? beta : alpha;
}

bool computer_go(BOARD *board, int color)
{
    int x, y;
    minmax(board, 1, color, x, y, color ? 1e8 : -1e8);
    board->board_put(x, y, color);
    if (color && board->checkban())
    {
        return false;
    }
    return true;
}

bool player_go(BOARD *board, int color)
{
    return computer_go(board, color);
    // int x = 0, y = 0;
    // while (!board->canput(x, y))
    // {
    //     cout << "Please input the coordinates:";
    //     cin >> x >> y;
    // }
    // board->board_put(x, y, color);
    // if (color && board->checkban())
    // {
    //     return false;
    // }
    // return true;
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