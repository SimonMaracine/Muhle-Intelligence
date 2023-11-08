#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <optional>
#include <cassert>

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

                const std::optional<Move> best_move {search_function()};

                // Play the move only if it was said so before the search
                if (best_move) {
                    play_move_and_save_position(*best_move);
                }

                // After search, reset the function as a signal
                search_function = {};
            }
        });
    }

    void MuhleImpl::new_game(const SmnPosition& position, const std::vector<Move>& moves) {
        reset_game();
        set_position(position, moves);
    }

    void MuhleImpl::go(Result& result, bool play_move) {
        assert(running);
        assert(!search_function);

        result = {};

        search_function = [this, &result, play_move]() {
            Search instance;
            instance.setup(parameters);  // TODO refactor

            assert(!game.previous_positions.empty());
            assert(game.previous_positions.size() - 1 == game.moves_played.size());

            // Get rid of the current position
            auto previous_positions {game.previous_positions};
            previous_positions.pop_back();

            const auto best_move {
                instance.search(
                    game.position,
                    previous_positions,
                    game.moves_played,
                    result
                )
            };

            return play_move ? std::make_optional(best_move) : std::nullopt;
        };

        cv.notify_one();
    }

    void MuhleImpl::move(const Move& move) {
        play_move_and_save_position(move);
    }

    void MuhleImpl::join_thread() {
        assert(running);

        // Set dummy work and set exit condition for stopping the thread
        search_function = []() { return std::nullopt; };
        running = false;

        cv.notify_one();

        thread.join();
    }

    void MuhleImpl::set_parameter(const std::string& parameter, int value) {
        parameters[parameter] = value;
    }

    void MuhleImpl::wait_for_work() {
        std::unique_lock<std::mutex> lock {mutex};
        cv.wait(lock, [this]() { return search_function; });
    }

    void MuhleImpl::reset_game() {
        game.position = {};
        game.previous_positions.clear();
        game.moves_played.clear();
    }

    void MuhleImpl::set_position(const SmnPosition& position, const std::vector<Move>& moves) {
        // Save this initial position too
        game.position = position;
        game.previous_positions.push_back(game.position);

        for (const Move& move : moves) {
            play_move_and_save_position(move);
        }
    }

    void MuhleImpl::play_move_and_save_position(const Move& move) {
        play_move(game.position, move);
        game.previous_positions.push_back(game.position);
        game.moves_played.push_back(move);
    }
}
