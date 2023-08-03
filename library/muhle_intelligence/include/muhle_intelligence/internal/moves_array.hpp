#pragma once

#include <array>
#include <cassert>

namespace muhle {
    template<typename T, unsigned int Size>
    class MovesArray {
    public:
        MovesArray() noexcept = default;
        ~MovesArray() noexcept = default;

        MovesArray(const MovesArray&) noexcept = default;
        MovesArray& operator=(const MovesArray&) noexcept = default;
        MovesArray(MovesArray&&) noexcept = default;
        MovesArray& operator=(MovesArray&&) noexcept = default;

        using const_iterator = typename std::array<T, Size>::const_iterator;

        void push_back(const T& value) noexcept {
            assert(data_size < Size);

            data[data_size] = value;
            data_size++;
        }

        unsigned int size() const noexcept {
            return data_size;
        }

        bool empty() const noexcept {
            return data_size == 0;
        }

        constexpr const_iterator begin() const noexcept {
            return const_iterator(data.data());
        }

        const_iterator end() const noexcept {
            return const_iterator(data.data() + data_size);
        }

        const T& operator[](unsigned int index) const noexcept {
            return data[index];
        }
    private:
        std::array<T, Size> data;
        unsigned int data_size = 0;
    };
}
