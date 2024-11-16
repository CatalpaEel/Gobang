#include <iostream>
#include <random>
using namespace std;
const int n = 15;

class BOARD
{
private:
    int board[16][16];
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

    bool is_in_board(int x, int y)
    {
        return x >= 1 && x <= n && y >= 1 && y <= n;
    }

    // Go from (x,y) in direction of dir until arrive (resx,resy) which is blank or the other color.
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
    bool canput(int x, int y)
    {
        return is_in_board(x, y) && board[x][y] == -1;
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

    // If (x,y) is banned, return true, which means that the black loses the game.
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
};

/*
class NODE
{
private:
    NODE *head;
    NODE *nxt;

public:
    int val;
    int x, y;
    int board[16][16];
    void evaluate()
    {
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (this->board[i][j] != -1)
                {
                    // RIGHT
                    for (int k = 1; k <= 5; ++k)
                    {
                    }
                }
            }
        }
    }
    void add_child(NODE *child)
    {
        child->nxt = this->head;
        this->head = child;
    }
};

class MINMAX_TREE
{
private:
    int color;
    const int max_dep = 5;

    NODE *root;
    void dfs(NODE *u, int dep, bool is_max)
    {
        if (dep == max_dep)
        {
            u->evaluate();
            return;
        }
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (canput(i, j, u->board))
                {
                    NODE *child = new NODE();
                    copy_board(u->board, child->board);
                    child->board[i][j] = color ^ is_max;
                    u->add_child(child);
                    dfs(child, dep + 1, is_max ^ 1);
                }
            }
        }
    }

    bool canput(int x, int y, int board[][n + 1])
    {
        return is_in_board(x, y) && board[x][y] == -1;
    }
    void copy_board(int old_board[][n + 1], int new_board[][n + 1])
    {
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                new_board[i][j] = old_board[i][j];
            }
        }
    }

public:
    void init(int color)
    {
        root = new NODE();
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                board[i][j] = -1;
            }
        }
        this->color = color;
        dfs(root, 1, true);
    }
    void get_next_position(int *x, int *y)
    {
    }
    void update_board(int x, int y)
    {
    }
} ;
*/

bool player_go(BOARD *board, int color)
{
    int x = 0, y = 0;
    while (!board->canput(x, y))
    {
        cout << "Please input the coordinates:";
        cin >> x >> y;
    }
    board->board_put(x, y, color);
    if (color && board->checkban(x, y))
    {
        return false;
    }
    return true;
}

bool computer_go(BOARD *board, int color)
{
    // todo
    return player_go(board, color);
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
    int black = rand() & 1, white = black ^ 1;
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
        cout << "White win!" << endl;
    else
        cout << "Black win" << endl;
}
int main()
{
    play();
}