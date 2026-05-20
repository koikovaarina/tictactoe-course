#include "my_player.hpp"
#include <cstdlib>

namespace ttt::my_player {

static const int CANDIDATE_RADIUS = 6; // радиус поиска перспективных клеток (только пустые клетки)
static const int MAX_CANDIDATES = 80; // макс количество кандидатов для верхнего уровня (после сортировки по эвристике)
static const int INNER_MAX_CANDIDATES = 30; // ограничивание кол-во кандидатов внутри дерева (для скорости)
static const int INNER_RADIUS = 3; // радиус поиска для внутренних узлов дерева минимакс

// направления
static const struct { int dx; int dy; } directions[] = {
    {1, 0}, {0, 1}, {1, 1}, {1, -1}
};

static bool within_boundaroes(int x, int y) {
    return x >= 0 && x < 20 && y >= 0 && y < 20;
}

struct GameState {
    Sign cells[20][20];
    Sign current_player;
    int turn_number;

    void initialize(const State& source) {
        for (int row_idx = 0; row_idx < 20; ++row_idx) {
            for (int col_idx = 0; col_idx < 20; ++col_idx) {
                cells[row_idx][col_idx] = source.get_value(col_idx, row_idx);
            }
        }
        current_player = source.get_current_player();
        turn_number = source.get_move_no();
    }

    Sign read(int x, int y) const {
        if (x < 0 || x >= 20 || y < 0 || y >= 20) {
            return Sign::WALL;
        }
        return cells[y][x];
    }

    void make_move(int x, int y) {
        cells[y][x] = current_player;
        current_player = (current_player == Sign::X) ? Sign::O : Sign::X;
        ++turn_number;
    }
};

static int evaluate_line(const Sign sequence[5], Sign my_player) {

    int mine = 0, theirs = 0, vacant = 0;
    int start_mine = -1, end_mine = -1;
    int start_vacant = -1, end_vacant = -1;
    
    for (int idx = 0; idx < 5; ++idx) {
        Sign current = sequence[idx];
        
        if (current == Sign::WALL) {
            return 0;
        }
        
        if (current == my_player) {
            ++mine;
            if (start_mine < 0) start_mine = idx;
            end_mine = idx;
        } else if (current == Sign::NONE) {
            ++vacant;
            if (start_vacant < 0) start_vacant = idx;
            end_vacant = idx;
        } else {
            ++theirs;
        }
    }
    
    if (mine > 0 && theirs > 0) {
        return 0;
    }
    
    bool left_free = (start_vacant == 0) || (start_mine == 0);
    bool right_free = (end_vacant == 4) || (end_mine == 4);
    int free_ends = (left_free ? 1 : 0) + (right_free ? 1 : 0);
    
    if (mine > 0 && theirs == 0) {
        switch (mine) {
            case 5:
                return 10000000;
            case 4:
                return (vacant == 1) ? (150000 + free_ends * 10000) : 0;
            case 3:
                if (vacant == 2) {
                    if (free_ends >= 2) return 25000;
                    if (free_ends == 1) return 8000;
                    return 1000;
                }
                return 0;
            case 2:
                return (vacant == 3) ? ((free_ends >= 2) ? 2000 : 400) : 0;
            case 1:
                return (vacant == 4 && free_ends >= 2) ? 100 : 10;
            default:
                return 0;
        }
    }
    
    if (theirs > 0 && mine == 0) {
        switch (theirs) {
            case 5:
                return -10000000;
            case 4:
                return (vacant == 1) ? -900000 : 0;
            case 3:
                if (vacant == 2) {
                    if (free_ends >= 2) return -150000;
                    if (free_ends == 1) return -40000;
                    return -5000;
                }
                return 0;
            case 2:
                return (vacant == 3) ? ((free_ends >= 2) ? -8000 : -1000) : 0;
            case 1:
                return (vacant == 4 && free_ends >= 2) ? -500 : -50;
            default:
                return 0;
        }
    }
    
    return 0;
}

static int score_all_light(const GameState& ls, Sign my_sign) {
    int total = 0;
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 20; ++x) {
            for (int dir = 0; dir < 4; ++dir) {
                Sign window[5];
                bool valid = true;
                for (int i = 0; i < 5; ++i) {
                    int nx = x + directions[dir].dx * i;
                    int ny = y + directions[dir].dy * i;
                    if (!within_boundaroes(nx, ny)) { valid = false; break; }
                    window[i] = ls.cells[ny][nx];
                }
                if (valid) total += evaluate_line(window, my_sign);
            }
        }
    }
    return total;
}

static int quick_score(const GameState& ls, int x, int y, Sign my_sign) {
    int total = 0;
    for (int dir = 0; dir < 4; ++dir) {
        for (int start = -4; start <= 0; ++start) {
            Sign window[5];
            bool valid = true;
            for (int i = 0; i < 5; ++i) {
                int nx = x + directions[dir].dx * (start + i);
                int ny = y + directions[dir].dy * (start + i);
                if (!within_boundaroes(nx, ny)) { valid = false; break; }
                window[i] = (nx == x && ny == y) ? my_sign : ls.cells[ny][nx];
            }
            if (valid) total += evaluate_line(window, my_sign);
        }
    }
    return total;
}

}; // namespace ttt::my_player
