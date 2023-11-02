#include <thread>
#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cassert>
#include <random>

#include "muhle_intelligence/muhle_intelligence.hpp"
#include "muhle_intelligence/internal/library.hpp"
#include "muhle_intelligence/internal/search.hpp"

namespace muhle {
    void MuhleImpl::initialize() {
        assert(!running);

        running = true;

        thread = std::thread([this]() {
            while (true) {
                wait_for_work();

                if (!running) {
                    break;
                }

                const Move best_move {search_function()};

                // Now save this position
                play_move(game.position, best_move);
                game.previous_positions.push_back(game.position.position);

                // After search reset the function as a signal
                search_function = {};
            }
        });
    }

    void MuhleImpl::new_game() {
        game.position = {};
        game.previous_positions.clear();
    }

    void MuhleImpl::position(const SmnPosition& position, const std::vector<Move>& moves) {
        // Set the position with the opponent move played
        game.position = position;
        game.previous_positions.push_back(game.position.position);

        for (const Move& move : moves) {
            play_move(game.position, move);
            game.previous_positions.push_back(game.position.position);
        }
    }

    void MuhleImpl::search(Result& result) {
        assert(running);
        assert(!search_function);

        result = {};

        search_function = [this, &result]() {
            Search instance;
            instance.setup(parameters);
            return instance.search(game.position, game.previous_positions, result);
        };

        cv.notify_one();
    }

    void MuhleImpl::join_thread() {
        assert(running);

        // Set dummy work and set exit condition for stopping the thread
        search_function = []() -> Move {};
        running = false;

        cv.notify_one();

        thread.join();
    }

    void MuhleImpl::set_parameter(std::string_view parameter, int value) {
        parameters[std::string(parameter)] = value;
    }

    void MuhleImpl::wait_for_work() {
        std::unique_lock<std::mutex> lock {mutex};
        cv.wait(lock, [this]() { return search_function; });
    }
}
