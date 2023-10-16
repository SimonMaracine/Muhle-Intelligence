#include <thread>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cassert>
#include <mutex>

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

                search_function();

                // After the search, reset the function
                search_function = {};
            }
        });
    }

    void MuhleImpl::search(const SearchInput& input, Result& result) {
        assert(running);
        assert(!search_function);

        result = {};

        search_function = [this, input, &result]() {
            Search instance;
            instance.setup(parameters);
            instance.search(input, result);
        };

        cv.notify_one();
    }

    void MuhleImpl::join_thread() {
        assert(running);

        // Set dummy work and set exit condition for stopping the thread
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
