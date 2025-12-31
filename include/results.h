#pragma once

#include "allocator.hpp"
#include "concepts.hpp"

namespace sde {

    template<concepts::FloatingPoint Num>
    class ArraySlice {
        Num* start;
        size_t _size;  //SIZE CONSIDERING STRIDE
        size_t stride;
    public:

        //ArraySlice() = default;

        ArraySlice(Num* start, size_t size, size_t stride = 1) : start(start), _size(size), stride(stride) {}

        Num& operator[](size_t n) {return *(start + (n*stride));}
        const Num& operator[](size_t n) const {return *(start + (n*stride));}

        Num& at(size_t n) {
            if(n >= _size) throw std::out_of_range("Index out of range");
            return *(start + (n*stride));
        }

        const Num& at(size_t n) const {
            if(n >= _size) throw std::out_of_range("Index out of range");
            return *(start + (n*stride));
        }

        [[nodiscard]] inline size_t size() const noexcept {return _size;}

        class Iterator {
            Num* ptr;
            size_t stride;

            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = Num;
            using pointer = Num*;
            using reference = Num&;

        public:
            Iterator(pointer ptr, size_t stride) : ptr(ptr), stride(stride) {}

            Iterator& operator++(){ptr += stride; return *this;}
            Iterator operator++(int){Iterator temp = *this; ++(*this); return temp;}

            Iterator& operator--(){ptr -= stride; return *this;}
            Iterator operator--(int){Iterator temp = *this; --(*this); return temp;}

            reference operator*() const {return *ptr;}

            Iterator operator+(difference_type n) const {return Iterator(ptr + n*stride, stride);}
            Iterator operator-(difference_type n) const {return Iterator(ptr - n*stride, stride);}
            friend Iterator operator+(difference_type n, const Iterator& it) { return it + n; }

            Iterator& operator+=(difference_type n){ptr += n*stride; return *this;}
            Iterator& operator-=(difference_type n){ptr -= n*stride; return *this;}

            difference_type operator-(Iterator other) const { return (ptr - other.ptr) / stride; }

            reference operator[](difference_type n) const {return *(ptr + n*stride);}

            bool operator==(const Iterator& other) const {return ptr == other.ptr;}
            bool operator!=(const Iterator& other) const {return !(*this == other);}

            bool operator<(const Iterator& other) const {return ptr < other.ptr;}
            bool operator>(const Iterator& other) const {return ptr > other.ptr;}

            bool operator<=(const Iterator& other) const {return ptr <= other.ptr;}
            bool operator>=(const Iterator& other) const {return ptr >= other.ptr;}
        };

        Iterator begin() const{return Iterator(start, stride);}
        Iterator end() const{return Iterator(start + _size*stride, stride);}
    };

    enum class Layout {
        TimeMajor,
        PathMajor
    };

    template<concepts::FloatingPoint Num>
    class array2d {
        Num* _data;
        size_t dim1, dim2, alignment;
        Layout layout;
        Num dt, t0;

    public:
        array2d() : _data(nullptr), dim1(0), dim2(0), alignment(1), layout(Layout::TimeMajor), dt(0), t0(0) {}

        array2d(size_t d1, size_t d2, Layout lay, Num dt, Num t0, size_t alignment = 1)
            : dim1(d1), dim2(d2), alignment(alignment), layout(lay), dt(dt), t0(t0)  {
            _data = memory::aligned_allocator<Num>().allocate(dim1 * dim2);
        }

        array2d(const array2d& other) {
            dim1 = other.dim1;
            dim2 = other.dim2;
            alignment = other.alignment;
            layout = other.layout;
            dt = other.dt;
            t0 = other.t0;
            _data = memory::aligned_allocator<Num>().allocate(dim1 * dim2);
            std::copy(other._data, other._data + dim1 * dim2, _data);
        }

        array2d& operator=(const array2d& other) {
            if (this == &other) return *this;
            dim1 = other.dim1;
            dim2 = other.dim2;
            alignment = other.alignment;
            layout = other.layout;
            dt = other.dt;
            t0 = other.t0;
            if (_data != nullptr) memory::aligned_allocator<Num>().deallocate(_data);
            _data = memory::aligned_allocator<Num>().allocate(dim1 * dim2);
            std::copy(other._data, other._data + dim1 * dim2, _data);
            return *this;
        }

        array2d(array2d&& other) noexcept {
            _data = other._data;
            dim1 = other.dim1;
            dim2 = other.dim2;
            alignment = other.alignment;
            layout = other.layout;
            dt = other.dt;
            t0 = other.t0;
            other._data = nullptr;
        }

        array2d& operator=(array2d&& other) noexcept {
            if (this == &other) return *this;
            if (_data != nullptr) memory::aligned_allocator<Num>().deallocate(_data);
            _data = other._data;
            dim1 = other.dim1;
            dim2 = other.dim2;
            alignment = other.alignment;
            layout = other.layout;
            dt = other.dt;
            t0 = other.t0;
            other._data = nullptr;
            return *this;
        }

        ~array2d() {
            if (_data == nullptr) return;
            memory::aligned_allocator<Num>().deallocate(_data);
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

        Iterator begin() const { return Iterator(_data); }
        Iterator end() const { return Iterator(_data + dim1*dim2); }

        [[nodiscard]] inline Layout get_layout() const { return layout; }

        [[nodiscard]] inline size_t get_dim1() const { return dim1; }
        [[nodiscard]] inline size_t get_dim2() const { return dim2; }
        [[nodiscard]] inline Num* data() {return _data;}

        class PathProxy {
            Num* row;
            size_t stride;
        public:
            PathProxy(Num* row, size_t stride) : row(row), stride(stride) {}
            Num& operator[](size_t n){return *(row + n*stride);}
            const Num& operator[](size_t n) const {return *(row + n*stride);}
        };

        PathProxy operator[](size_t path) {
            if (layout == Layout::PathMajor) {
                return PathProxy(_data + path * dim2, 1);
            }
            return PathProxy(_data + path, dim2);
        }

        const PathProxy operator[](size_t path) const {
            if (layout == Layout::PathMajor) {
                return RowProxy(_data + path * dim2, 1);
            }
            return RowProxy(_data + path, dim2);
        }

        Num& operator()(size_t path, size_t time) {
            if (layout == Layout::PathMajor) {
                return _data[path * dim2 + time];
            }
            return _data[path + time * dim1];
        }

        const Num& operator()(size_t path, size_t time) const {
            if (layout == Layout::PathMajor) {
                return _data[path * dim2 + time];
            }
            return _data[path + time * dim1];
        }

        Num& get_raw(size_t row, size_t col) {
            return _data[row * dim2 + col];
        }

        const Num& get_raw(size_t row, size_t col) const {
            return _data[row * dim2 + col];
        }

        Num& at(size_t path, size_t time) {
            if (layout == Layout::PathMajor) {
                if (path >= dim1 || time >= dim2) throw std::out_of_range("Index out of range");
                return _data[path * dim2 + time];
            }
            if (path >= dim2 || time >= dim1) throw std::out_of_range("Index out of range");
            return _data[path + time * dim1];
        }

        const Num& at(size_t path, size_t time) const {
            if (layout == Layout::PathMajor) {
                if (path >= dim1 || time >= dim2) throw std::out_of_range("Index out of range");
                return _data[path * dim2 + time];
            }
            if (path >= dim2 || time >= dim1) throw std::out_of_range("Index out of range");
            return _data[path + time * dim1];
        }

        Num& at_raw(size_t row, size_t col) {
            if (row >= dim1 || col >= dim2) throw std::out_of_range("Index out of range");
            return _data[row * dim2 + col];
        }

        const Num& at_raw(size_t row, size_t col) const {
            if (row >= dim1 || col >= dim2) throw std::out_of_range("Index out of range");
            return _data[row * dim2 + col];
        }
        ArraySlice<Num> get_path(size_t path) {
            if (layout == Layout::PathMajor) {
                if (path >= dim1) throw std::out_of_range("Index out of range");
                return ArraySlice<Num>(_data + path * dim2, dim2, 1);
            }
            if (path >= dim2) throw std::out_of_range("Index out of range");
            return ArraySlice<Num>(_data + path, dim1, dim2);
        }


        ArraySlice<Num> get_timestep(size_t step) {
            if (layout == Layout::PathMajor) {
                if (step >= dim2) throw std::out_of_range("Index out of range");
                return ArraySlice<Num>(_data + step, dim1, dim2);
            }
            if (step >= dim1) throw std::out_of_range("Index out of range");
            return ArraySlice<Num>(_data + step * dim2, dim2, 1);
        }

        ArraySlice<Num> get_time(Num time) {
            return get_timestep(static_cast<size_t>((time - t0) / dt));
        }

        array2d<Num> transpose_copy() {
            array2d<Num> result;
            if (array2d::layout == Layout::PathMajor) {
                result = array2d(dim2, dim1, Layout::TimeMajor, dt, t0, alignment);
            }else {
                result = array2d(dim2, dim1, Layout::PathMajor, dt, t0, alignment);
            }
            for (size_t i = 0; i < dim1; ++i) {
                for (size_t j = 0; j < dim2; ++j) {
                    result.get_raw(i,j) = get_raw(j,i);
                }
            }
            return result;
        }

        void transpose_inplace() {
            throw std::runtime_error("Transpose inplace not implemented yet");
        }
        array2d<Num> transpose_view() {
            throw std::runtime_error("Transpose view not implemented yet");
        }

        array2d<Num> subgraph(size_t path_start, size_t path_end, size_t time_start, size_t time_end) {
            throw std::runtime_error("Subgraph not implemented yet");
        }

        array2d<Num> subgraph(size_t path_end, size_t time_end) {
            throw std::runtime_error("Subgraph not implemented yet");
        }
    };
}