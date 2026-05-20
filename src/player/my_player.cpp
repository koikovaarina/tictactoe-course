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

}; // namespace ttt::my_player
