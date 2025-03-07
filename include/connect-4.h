#ifndef CONNECT_4_H
#define CONNECT_4_H

#include <vector>
#include <array>
#include <string>
#include <limits>

class State {
    public:
    static const int ROWS = 6;
    static const int COLS = 7;
    const std::vector<int> prioritized_cols = {3, 2, 4, 1, 5, 0, 6};
    int board[COLS][ROWS];
    int current_player;
    int turn;

    State();
    void boardInit();
    State(int grid[COLS][ROWS]);
    std::string toString();
    bool canPlace(int column);
    bool place(int column);
    int isAWin();
    int negamax(int alpha, int beta);
    int bestMove();
    int overruleCheck(bool current_move);
    bool unplace(int column);
};

#endif