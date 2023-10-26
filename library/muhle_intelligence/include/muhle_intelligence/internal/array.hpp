#pragma once

#include <cassert>
#include <iterator>
#include <cstddef>

namespace muhle {
    template<typename T, std::size_t Size>
    class Array {
    public:
        Array() noexcept = default;
        ~Array() noexcept = default;

        Array(const Array&) noexcept = default;
        Array& operator=(const Array&) noexcept = default;
        Array(Array&&) noexcept = default;
        Array& operator=(Array&&) noexcept = default;

        void push_back(const T& value) noexcept {
            assert(data_size < Size);

            data[data_size] = value;
            data_size++;
        }

        const T& operator[](std::size_t index) const noexcept {
            return data[index];
        }

        std::size_t size() const noexcept {
            return data_size;
        }

        bool empty() const noexcept {
            return data_size == 0;
        }

        class const_iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using const_reference = const value_type&;

            explicit const_iterator(const_pointer ptr) noexcept
                : ptr(ptr) {}

            const_iterator& operator++() noexcept {
                ptr++;

                return *this;
            }

            const_iterator operator++(int) noexcept {
                const_iterator temporary {*this};

                ++(*this);

                return temporary;
            }

            bool operator==(const_iterator other) const noexcept {
                return ptr == other.ptr;
            }

            bool operator!=(const_iterator other) const noexcept {
                return ptr != other.ptr;
            }

            const_pointer operator->() const noexcept {
                return ptr;
            }

            const_reference operator*() const noexcept {
                return *ptr;
            }
        private:
            const_pointer ptr;
        };

        constexpr const_iterator begin() const noexcept {
            return const_iterator(data);
        }

        const_iterator end() const noexcept {
            return const_iterator(data + data_size);
        }
    private:
        T data[Size];
        std::size_t data_size {0};
    };
}
