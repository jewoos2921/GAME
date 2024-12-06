//
// Created by eu on 2024-12-01.
//
#include <cassert>
#include <random>
#include <iostream>
#include <vector>
#include <sstream>
#include <queue>
#include <chrono>

namespace HillClimb
{
    using ScoreType = int64_t;
    constexpr const ScoreType INF = 1000000000LL;
    std::mt19937 mt_for_action(0);

    struct Coord
    {
        int y_;
        int x_;

        Coord(const int y = 0, const int x = 0): y_(y), x_(x)
        {
        }
    };

    constexpr const int H{5};
    constexpr const int W{5};
    constexpr int END_TURN{5};
    constexpr int CHARACTER_N{3};

    class TimeKeeper
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
        int64_t time_threshold_;

    public:
        // 시간 제한을 밀리초 단위로 지정해서 인스턴스르 생성
        TimeKeeper(const int64_t& time_threshold): start_time_(std::chrono::high_resolution_clock::now()),
                                                   time_threshold_(time_threshold)
        {
        }

        bool isTimeOver() const
        {
            using std::chrono::duration_cast;
            using std::chrono::milliseconds;
            auto diff = std::chrono::high_resolution_clock::now() - this->start_time_;
            return duration_cast<milliseconds>(diff).count() >= time_threshold_;
        }
    };

    class AutoMoveMazeState
    {
    private:
        int points_[H][W] = {};
        int turn_{0};

        Coord characters_[CHARACTER_N] = {};

        static constexpr const int dx[4] = {1, -1, 0, 0};
        static constexpr const int dy[4] = {0, 0, 1, -1};

    public:
        int game_score_ = 0; // 게임에서 획득한 점수

        ScoreType evaluated_score_{}; // 탐색을 통해 확인한 점수

        // h*w 크기의 미로를 생성
        explicit AutoMoveMazeState(const int seed) : turn_(0), game_score_(0), evaluated_score_(0)
        {
            auto mt_for_construct = std::mt19937(seed);
            for (int y = 0; y < H; ++y)
            {
                for (int x = 0; x < W; ++x)
                {
                    points_[y][x] = mt_for_construct() % 9 + 1;
                }
            }
        }

        void setCharacter(const int character_id, const int y, const int x)
        {
            this->characters_[character_id].y_ = y;
            this->characters_[character_id].x_ = x;
        }

        std::string toString() const
        {
            std::stringstream ss;
            ss << "turn:\t" << this->turn_ << "\n";
            ss << "score:\t" << this->game_score_ << "\n";
            for (int h = 0; h < H; h++)
            {
                for (int w = 0; w < W; w++)
                {
                    if (this->characters_->y_ == h && this->characters_->x_ == w)
                    {
                        ss << '@';
                    }
                    else if (this->points_[h][w] > 0)
                    {
                        ss << points_[h][w];
                    }
                    else
                    {
                        ss << ".";
                    }
                }
                ss << "\n";
            }
            return ss.str();
        }

        bool isDone() const
        {
            return this->turn_ == END_TURN;
        }

        ScoreType getScore(bool is_print = false) const
        {
            auto tmp_state = *this;
            // 캐릭터 위치에 있는 점수를 삭제한다.
            for (auto& character : this->characters_)
            {
                auto& point = tmp_state.points_[character.y_][character.x_];
                point = 0;
            }
            // 종료할 때까지 캐릭터 이동을 반복한다
            while (!tmp_state.isDone())
            {
                tmp_state.advance();
                if (is_print)
                    std::cout << tmp_state.toString() << std::endl;
            }
            return tmp_state.game_score_;
        }

        void movePlayer(const int character_id)
        {
            Coord& character = this->characters_[character_id];
            int best_point = -INF;
            int best_action_index = 0;
            for (int action = 0; action < 4; ++action)
            {
                int ty = character.y_ + dy[action];
                int tx = character.x_ + dx[action];
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                {
                    auto point = this->points_[ty][tx];
                    if (point > best_point)
                    {
                        best_point = point;
                        best_action_index = action;
                    }
                }
            }
            character.y_ += dy[best_action_index];
            character.x_ += dx[best_action_index];
        }

        void advance()
        {
            for (int character_id = 0; character_id < CHARACTER_N; ++character_id)
            {
                movePlayer(character_id);
            }
            for (auto& character : this->characters_)
            {
                auto& point = this->points_[character.y_][character.x_];
                this->game_score_ += point;
                point = 0;
            }
            ++this->turn_;
        }

        void init()
        {
            for (auto& character : this->characters_)
            {
                character.y_ = mt_for_action() % H;
                character.x_ = mt_for_action() % W;
            }
        }

        void transition()
        {
            auto& character = this->characters_[mt_for_action() % CHARACTER_N];
            character.y_ = mt_for_action() % H;
            character.x_ = mt_for_action() % W;
        }
    };

    using State = AutoMoveMazeState;


    State randomAction(const State& state)
    {
        State now_state = state;
        for (int character_id = 0; character_id < CHARACTER_N; ++character_id)
        {
            int y = mt_for_action() % H;
            int x = mt_for_action() % W;

            now_state.setCharacter(character_id, y, x);
        }
        return now_state;
    }

    State hillClimb(const State& state, int number)
    {
        State now_state = state;
        now_state.init();
        ScoreType best_score = now_state.getScore();
        for (int i = 0; i < number; ++i)
        {
            auto next_state = now_state;
            next_state.transition();
            auto next_score = next_state.getScore();
            if (next_score > best_score)
            {
                best_score = next_score;
                now_state = next_state;
            }
        }
        return now_state;
    }

    void playGame(const StringAIPair& ai, const int seed)
    {
        auto state = State(seed);
        state = ai.second(state);
        std::cout << state.toString() << std::endl;
        auto score = state.getScore(true);
        std::cout << "Score of " << ai.first << " : " << score << std::endl;
    }

    int make_action()
    {
        const auto& ai = StringAIPair("hillClimb", [&](const State& state) { return hillClimb(state, 10000); });
        playGame(ai, 0);
        return 0;
    }
}
