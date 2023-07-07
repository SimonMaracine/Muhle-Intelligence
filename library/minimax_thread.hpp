#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <utility>

#include "common.hpp"

class MinimaxThread {
public:
    MinimaxThread(std::unique_ptr<MinimaxAlgorithm>&& minimax)
        : minimax(std::move(minimax)) {}

    ~MinimaxThread();

    MinimaxThread(const MinimaxThread&) = delete;
    MinimaxThread& operator=(const MinimaxThread&) = delete;
    MinimaxThread(MinimaxThread&&) = delete;
    MinimaxThread& operator=(MinimaxThread&& other) noexcept;

    void start(const Position& position, Piece piece);

    bool is_running() const;
    void join();

    const Move& get_result() const { return result; }
private:
    void join_thread();

    std::thread thread;
    std::atomic<bool> running = false;
    std::unique_ptr<MinimaxAlgorithm> minimax;

    Move result;
};