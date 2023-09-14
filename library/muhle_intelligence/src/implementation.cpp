#include <thread>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cassert>
#include <mutex>

#include "muhle_intelligence/internal/implementation.hpp"
#include "muhle_intelligence/internal/array.hpp"
#include "muhle_intelligence/muhle_intelligence.hpp"

#include "muhle_intelligence/internal/variants/standard_game.hpp"

namespace muhle {
    template<typename T>
    static void search_instance(std::unordered_map<std::string, int>& parameters, const Position& position, Player player, Result& result) {
        T search;
        search.setup(parameters);
        search.figure_out_position(position);
        search.search(player, result);
    }

    void MuhleImpl::initialize(Game game) {
        assert(!running);

        this->game = game;

        running = true;

        thread = std::thread([this]() {
            while (true) {
                wait_for_work();

                if (!running) {
                    break;
                }

                search_function();

                // After the search, reset the function
                search_function = {};
            }
        });
    }

    void MuhleImpl::search(const Position& position, Player player, Result& result) {
        assert(running);
        assert(!search_function);

        result = {};

        switch (game) {
            case Game::StandardGame:
                search_function = [this, position, player, &result]() {
                    search_instance<StandardGame>(parameters, position, player, result);
                };
                break;
        }

        cv.notify_one();
    }

    void MuhleImpl::join_thread() {
        assert(running);

        // Set dummy work and exit condition for stopping the thread
        search_function = []() {};
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
