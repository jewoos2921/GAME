//
// Created by eu on 2024-12-01.
//
#include <assert.h>
#include <random>
#include <iostream>
#include <vector>
#include <sstream>

namespace Greedy {
    using ScoreType = int64_t;
    constexpr const ScoreType INF = 1000000000LL;

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

        void advance(const int action) {
            this->character_.x_ += dx[action];
            this->character_.y_ += dy[action];
            auto &point = this->points_[this->character_.y_][this->character_.x_];
            if (point > 0) {
                this->game_score_ += point;
                point = 0;
            }
            this->turn_++;
        }

        // 현재 상황에서 플레이어가 가능한 행동을 모두 획득한다.
        std::vector<int> legalActions() const {
            std::vector<int> actions;
            for (int action = 0; action < 4; action++) {
                int ty = this->character_.y_ + dy[action];
                int tx = this->character_.x_ + dx[action];
                if (ty >= 0 && ty < H && tx >= 0 && tx < W) {
                    actions.emplace_back(action);
                }
            }
            return actions;
        }

        // 현재 게임 상황을 문자열로 만든다.
        std::string toString() const {
            std::stringstream ss;
            ss << "turn:\t" << this->turn_ << "\n";
            ss << "score:\t" << this->game_score_ << "\n";
            for (int h = 0; h < H; h++) {
                for (int w = 0; w < W; w++) {
                    if (this->character_.y_ == h && this->character_.x_ == w) {
                        ss << '@';
                    } else if (this->points_[h][w] > 0) {
                        ss << points_[h][w];
                    } else {
                        ss << ".";
                    }
                }
                ss << "\n";
            }
            return ss.str();
        }

    public:
        // 탐색을 통해 확인한 점수
        ScoreType evaluated_score_ = 0;
        // 탐색용으로 게임판을 평가
        void evaluateScore() {
            // 간단히 우선 기록 점수를 그대로 게임판의 평가로 사용
            this->evaluated_score_ = this->game_score_;
        }
    };

    std::mt19937 mt_for_action(0);

    int randomAction(const MazeState &state) {
        auto legal_actions = state.legalActions();
        return legal_actions[mt_for_action() % legal_actions.size()];
    }

    int greedyAction(const MazeState &state) {
        auto legal_actions = state.legalActions();
        ScoreType best_score = -INF;
        int best_action = -1;
        for (const auto action: legal_actions) {
            MazeState now_state = state;
            now_state.advance(action);
            now_state.evaluateScore();
            if (now_state.evaluated_score_ > best_score) {
                best_score = now_state.evaluated_score_;
                best_action = action;
            }
        }
        assert(best_action !=-1);
        return best_action;
    }

    void playGame(const int seed) {
        auto state = MazeState(seed);
        std::cout << state.toString() << "\n";
        while (!state.isDone()) {
            state.advance(greedyAction(state));
            std::cout << state.toString() << "\n";
        }
    }

    void testAiScore(const int game_number) {
        std::mt19937 mt_for_construct(0);
        double score_mean{0};
        for (int i = 0; i < game_number; i++) {
            auto state = MazeState(mt_for_construct());
            while (!state.isDone()) {
                state.advance(greedyAction(state));
            }
            auto score = state.game_score_;
            score_mean += score;
        }
        score_mean /= (double) game_number;
        std::cout << "Score:\t" << score_mean << "\n";
    }
}
