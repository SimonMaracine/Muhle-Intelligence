#pragma once

#include <thread>
#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>

#include <muhle_intelligence/definitions.hpp>

#include "muhle_intelligence/muhle_intelligence.hpp"
#include "muhle_intelligence/internal/various.hpp"

namespace muhle {
    class MuhleImpl : public MuhleIntelligence {
    public:
        virtual void initialize() override;
        virtual void new_game() override;
        virtual void position(const SmnPosition& position, const std::vector<Move>& moves) override;
        virtual void search(Result& result) override;
        virtual void join_thread() override;
        virtual void set_parameter(std::string_view parameter, int value) override;
    private:
        void wait_for_work();

        std::thread thread;
        std::function<void()> search_function;
        bool running {false};

        std::condition_variable cv;
        std::mutex mutex;

        std::unordered_map<std::string, int> parameters;

        struct {
            SmnPosition position;
            std::vector<Position> previous_positions;
        } game;
    };
}
