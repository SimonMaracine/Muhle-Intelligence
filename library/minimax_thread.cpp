#include <thread>

#include "minimax_thread.hpp"
#include "common.hpp"

MinimaxThread::~MinimaxThread() {
    join_thread();
}

MinimaxThread& MinimaxThread::operator=(MinimaxThread&& other) noexcept {
    join_thread();

    thread = std::move(other.thread);
    running.store(other.running.load());

    result = other.result;

    return *this;
}

void MinimaxThread::start(const Position& position, Piece piece) {
    running.store(true);
    result = Move();

    thread = std::thread([this, &position, piece]() {  // TODO spawn only one thread and reuse it
        minimax->search(position, piece, result, running);
    });
}

bool MinimaxThread::is_running() const {
    return running.load();
}

void MinimaxThread::join() {
    join_thread();
}

void MinimaxThread::join_thread() {
    if (thread.joinable()) {
        thread.join();
    }
}
