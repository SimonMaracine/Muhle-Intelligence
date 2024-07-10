#pragma once

#include <string>
#include <stdexcept>

namespace subprocess {
    struct Error : public std::runtime_error {
        explicit Error(const char* message)
            : std::runtime_error(message) {}
        explicit Error(const std::string& message)
            : std::runtime_error(message) {}
    };

    class Subprocess {
    public:
        Subprocess() noexcept = default;
        explicit Subprocess(const std::string& file_path);
        ~Subprocess() noexcept;

        Subprocess(const Subprocess&) = delete;
        Subprocess& operator=(const Subprocess&) = delete;
        Subprocess(Subprocess&& other) noexcept;
        Subprocess& operator=(Subprocess&& other) noexcept;

        bool read_from(std::string& data) const;
        bool write_to(const std::string& data) const;
        bool wait_for() noexcept;
    private:
        int input {};  // Read from
        int output {};  // Write to
        int child_pid {-1};

        mutable std::string buffered;
    };
}
