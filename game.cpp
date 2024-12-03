#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "jsoncpp/json.h"

using namespace std;
const int n = 15;
clock_t START_TIME;

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

    void printboard()
    {
        cout << "--------------------------" << endl;
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
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
        if (len == 5)
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

int evaluate(BOARD *board, int tx, int ty)
{
    if (board->get_color(tx, ty) == 1 && board->checkban(tx, ty))
    {
        return -1e7;
    }

    int sum = 0; // {white ,black}
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
            sum += val;
        }
    }
    return sum;
}

int minmax(BOARD *board, int dep, int color, int &x, int &y, int front)
{
    const int MAX_DEP = 5;
    const int MAX_CHILD = 50;
    const int INF = 1e8;

    if (x != -1)
    {
        if (dep >= MAX_DEP)
        {
            return evaluate(board, x, y);
        }
        if (1.0 * (clock() - START_TIME) / CLOCKS_PER_SEC > 0.9) // time breaker
        {
            return evaluate(board, x, y);
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
                int val = evaluate(board, i, j) * (MAX_DEP - dep + 1);
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

bool computer_go(BOARD *board, int color)
{
    int x, y;
    minmax(board, 1, color, x = -1, y = -1, color ? 1e8 : -1e8);
    board->board_put(x, y, color);
    if (color && board->checkban(x, y))
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
    // if (color && board->checkban(x, y))
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
        if (board->full())
        {
            cout << "Drawn game" << endl;
            return;
        }
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

int main()
{
    botzone_play();
}