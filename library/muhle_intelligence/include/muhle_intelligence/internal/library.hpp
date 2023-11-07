#pragma once

#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <optional>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/muhle_intelligence.hpp"
#include "muhle_intelligence/internal/various.hpp"

namespace muhle {
    class MuhleImpl : public MuhleIntelligence {
    public:
        virtual void initialize() override;
        virtual void new_game(const SmnPosition& position = {}, const std::vector<Move>& moves = {}) override;
        virtual void go(Result& result, bool play_move = true) override;
        virtual void move(const Move& move) override;
        virtual void join_thread() override;
        virtual void set_parameter(const std::string& parameter, int value) override;
    private:
        void wait_for_work();
        void reset_game();
        void set_position(const SmnPosition& position, const std::vector<Move>& moves);
        void play_move_and_save_position(const Move& move);

        std::thread thread;
        std::function<std::optional<Move>()> search_function;
        bool running {false};

        std::condition_variable cv;
        std::mutex mutex;

        std::unordered_map<std::string, int> parameters;

        struct {
            SmnPosition position;
            std::vector<SmnPosition> previous_positions;  // Always contains the current position too
            std::vector<Move> moves_played;
        } game;
    };
}
