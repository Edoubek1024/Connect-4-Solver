#include "../include/connect-4.h"
#include "../include/openings.h"
#include <map>
#include <fstream>

// Initializing openings here allows for direct modification within the file
std::map<std::string, int> openings;

// A board state initializes with an empty grid, turn set to 0, and current_player set to 1
// turn is set to 0 since no one has made any placements
// current_player is set to 1 initially. It is either equal to 1 or -1 and alternates upon placements
State::State() {
    turn = 0;
    current_player = 1;
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            board[x][y] = 0;
        }
    }
}

// State can also be initialized with a board grid
// All variables then result from the processing of that grid
State::State(int grid[COLS][ROWS]) {
    current_player = 1;
    turn = 0;
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            board[x][y] = grid[x][y];
            if (board[x][y] != 0) {
                current_player *= -1;
                turn++;
            }
        }
    }
}

// This is used only for hashing
// All keys in openings.json are strings resulting from this function
std::string State::toString() {
    std::string hash_string;
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            hash_string += (board[x][y] == -1) ? "2" : std::to_string(board[x][y]);
        }
    }
    return hash_string;
}

// This function determines if a certain column of the board state is full or not
// If a piece can still be legally played there, it returns true, else false
bool State::canPlace(int column) {
    return (column >= 0 && column < COLS && board[column][0] == 0);
}

// This places a piece in whichever column is inputted
// Once this is done, the board state switches so that the other player can now move
bool State::place(int column) {
    if (!canPlace(column)) {
        return false;
    }
    for (int i = 0; i < ROWS; i++) {
        if (i == (ROWS - 1) || board[column][i + 1] != 0) {
            board[column][i] = current_player;
            break;
        }
    }
    current_player *= -1;
    turn++;
    return true;
}

// The unplace function does the opposite of the place function
// This is used only within the negamax determination in order to increase efficiency
bool State::unplace(int column) {
    for (int i = 0; i < ROWS; i++) {
        if (board[column][i] != 0) {
            board[column][i] = 0;
            current_player *= -1;
            turn--;
            return true;
        }
    }
    return false;
}

// The isAWin function determines if anyone has won the game yet
// If there are any instances of 4 pieces in a row diagonally, horizontally, or vertically, that player has won
// This returns 0 if there is no win, 1 if Player 1 has won, and -1 if Player 2 has won
int State::isAWin() {
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            if (x < (COLS - 3) && board[x][y] != 0 && board[x][y] == board[x + 1][y] && board[x][y] == board[x + 2][y] && board[x][y] == board[x + 3][y]) {
                return (board[x][y]);
            }
            if (y < ROWS - 3) {
                if (board[x][y] != 0 && board[x][y] == board[x][y + 1] && board[x][y] == board[x][y + 2] && board[x][y] == board[x][y + 3]) {
                    return board[x][y];
                }
                if (x < COLS - 3 && board[x][y] != 0 && board[x][y] == board[x + 1][y + 1] && board[x][y] == board[x + 2][y + 2] && board[x][y] == board[x + 3][y + 3]) {
                    return board[x][y];
                }
                if (x > 2 && board[x][y] != 0 && board[x][y] == board[x - 1][y + 1] && board[x][y] == board[x - 2][y + 2] && board[x][y] == board[x - 3][y + 3]) {
                    return board[x][y];
                }
            }
        }
    }
    return 0;
}

// This function allows for both a win check and a threat check
// If the current player can win immediately, then the rest of the negamax algorithm is unnecessary
// If an immediate win is not possible, then this function makes a threat check
// The threat check increases efficiency by immediately checking if the current player needs to block a win from the other player
int State::overruleCheck(bool current_move) {
    // Win check
    for (int i : prioritized_cols) {
        if (!canPlace(i)) {
            continue;
        }
        place(i);
        if (isAWin()) {
            unplace(i);
            return i;
        }
        unplace(i);
    }

    // Threat check
    if (current_move) {
        for (int i : prioritized_cols) {
            if (!canPlace(i)) {
                continue;
            }
            current_player *= -1;
            place(i);
            if (isAWin()) {
                unplace(i);
                current_player *= -1;
                return i;
            }
            unplace(i);
            current_player *= -1;
        }
    }
    return -1;
}

// This negamax algorithm is what determines the evaluation scores of any leaf nodes within the game tree
// By implementing alpha-beta pruning and other filters to increase efficiency, this function works efficiently
// to determine the most optimal evaluation scores in order to make the strongest move
int State::negamax(int alpha, int beta) {
    // If the board state is an instance of a winning board or a completely filled board,
    // a score, weighted by the depth of the game tree, is returned
    // Since this is a leaf node of a negamax function, a winner will always be the opposite of the current player,
    // so a negative score is returned. If the board is filled, a 0 value is returned
    int winner = isAWin();
    if (winner != 0 || turn == COLS*ROWS) {
        return (winner == 0) ? 0 : -10*((COLS*ROWS + 1) - turn);
    }

    // This is a feature of the alpha-beta pruning algorithm which evaluates if there are any possible
    // scores past this depth that are more optimal than already found options.
    // If not, all child nodes are pruned
    if (10*((COLS*ROWS) - turn) < alpha) {
        return 0;
    }
    // If a state can result in an immediate win, a corresponding value is returned, therefore pruning any child nodes
    int immediate_win = overruleCheck(false);
    if (immediate_win != -1) {
        return 10*(COLS*ROWS - turn);
    }

    // If all else fails, the algorithm searches through child nodes utilizing alpha-beta pruning
    // In order to increase efficiency, the algorithm uses a board state reference, requiring placing and unplacing
    int maxValue = std::numeric_limits<int>::min();
    for (int i : prioritized_cols) {
        if (!canPlace(i)) {
            continue;
        }
        place(i);
        int score = -negamax(-beta, -alpha);
        unplace(i);
        if (score > maxValue) {
            maxValue = score;
        }
        // Alpha-beta pruning
        alpha = std::max(alpha, score);
        if (alpha >= beta) {
            break;
        }
    }
    return maxValue;
}

// This function returns the optimal move index and begins the negamax search
int State::bestMove() {

    // If the current state's optimal move is tracked in the openings map, it returns the mapped index
    if (openings.find(toString()) != openings.end()) {
        return openings[toString()];
    }

    // For security, a copy of this board state is made
    State temp_state = *this;

    // Checks for both immediate wins and threats
    int immediate_win = temp_state.overruleCheck(true);
    if (immediate_win != -1) {
        return immediate_win;
    }

    // Begins the negamax search
    int high_score = -10000;
    int best = -1;

    for (int i : prioritized_cols) {
        if (!canPlace(i)) {
            continue;
        }
        temp_state.place(i);
        int score = -temp_state.negamax(-10000, 10000);
        temp_state.unplace(i);

        if (score > high_score) {
            high_score = score;
            best = i;
        }
    }
    // Maps the current state to its optimal move for increased efficiency in the future
    openings[toString()] = best;
    return best;
}
