#include <random>
//
// Created by eu on 2024-12-01.
//
namespace MazeState {
    struct Coord {
        int y_;
        int x_;

        Coord(const int y = 0, const int x = 0): y_(y), x_(x) {
        }
    };

    constexpr const int H{3};
    constexpr const int W{4};
    constexpr int END_TURN{4};

    class MazeState {
    private:
        int points_[H][W] = {};
        int turn_{0};

        static constexpr const int dx[4] = {1, -1, 0, 0};
        static constexpr const int dy[4] = {0, 0, 1, -1};

    public:
        Coord character_ = Coord(0, 0);
        int game_score_ = 0;

        MazeState() = default;

        MazeState(const int seed) {
            auto mt_for_construct = std::mt19937(seed);
            this->character_.y_ = mt_for_construct() % H;
            this->character_.x_ = mt_for_construct() % W;

            for (int y = 0; y < H; y++) {
                for (int x = 0; x < W; x++) {
                    if (y == character_.y_ && x == character_.x_) {
                        continue;
                    }
                    this->points_[y][x] = mt_for_construct() % 10;
                }
            }
        }

        bool isDone() const {
            return this->turn_ == END_TURN;
        }
    };
}
