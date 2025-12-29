#pragma once

#include "allocator.hpp"
#include "concepts.hpp"

namespace sde {

    enum class Layout {
        TimeMajor,
        PathMajor
    };

    template<concepts::FloatingPoint Num>
    class sde_2darray {
        Num* data;
        size_t dim1, dim2, alignment;
        Layout layout;

    public:
        sde_2darray(size_t d1, size_t d2, Layout lay, size_t alignment = 1)
            : dim1(d1), dim2(d2), alignment(alignment), layout(lay) {
            data = memory::aligned_allocator<Num>().allocate(alignment, dim1 * dim2);
        }

        ~sde_2darray() {
            memory::aligned_allocator<Num>().deallocate(data);
        }



        class Iterator {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = Num;
            using pointer = Num*;
            using reference = Num&;

            pointer ptr;
        public:
            explicit Iterator(pointer ptr) : ptr(ptr) {}

            pointer operator->() { return ptr; }
            reference operator*() { return *ptr; }

            Iterator& operator++(){ptr++; return *this;}
            Iterator operator++(int){Iterator tmp = *this; ++(*this); return tmp;}

            Iterator& operator--(){ptr--; return *this;}
            Iterator operator--(int){Iterator tmp = *this; --(*this); return tmp;}

            Iterator& operator+=(difference_type n){ptr += n; return *this;}
            Iterator& operator-=(difference_type n){ptr -= n; return *this;}

            Iterator operator+(difference_type n) const {return Iterator(ptr + n); }
            Iterator operator-(difference_type n) const {return Iterator(ptr - n); }
            friend Iterator operator+(difference_type n, const Iterator& it) { return it + n; }

            difference_type operator-(const Iterator& it) const { return ptr - it.ptr; }

            reference operator[](difference_type n) const { return ptr[n]; }

            auto operator<=>(const Iterator& other) const = default;

        };

        Iterator begin() const { return Iterator(data); }
        Iterator end() const { return Iterator(data + dim1*dim2); }

        [[nodiscard]] inline Layout get_layout() const { return layout; }

        [[nodiscard]] inline size_t get_dim1() const { return dim1; }
        [[nodiscard]] inline size_t get_dim2() const { return dim2; }
        [[nodiscard]] inline Num* data() {return data;}

    };
}