#ifndef UNORTHODOX_BUFFER_HPP
#define UNORTHODOX_BUFFER_HPP

#include <string>
#include <compare>
#include <unistd.h>

#include "util.hpp"

namespace unorthodox
{
    class buffer
    {
        public:
            template <bool is_const> class iterator_type;

            using value_type        = std::byte;
            using size_type         = std::size_t;
            using difference_type   = std::ptrdiff_t;
            using reference         = std::byte&;
            using const_reference   = const std::byte&;
            using pointer           = std::byte*;
            using const_pointer     = const std::byte*;

            using iterator          = iterator_type<false>;
            using const_iterator    = iterator_type<true>;

            constexpr static size_type GROW_MULTIPLIER = 2;
            constexpr static size_type DEFAULT_LOCATION = ~0;

            explicit buffer() noexcept = default;

            constexpr buffer(const buffer& other) noexcept;
            constexpr buffer(buffer&& other) noexcept;

            template <typename InputIt>
            constexpr buffer(InputIt first, InputIt last) noexcept;

            template <typename Iterable>
            constexpr buffer(const Iterable& source) noexcept : buffer(source.begin(), source.end()) {}

            buffer(const char* src) noexcept;
            buffer(char* src) noexcept;

           ~buffer();

            // Assignment
            constexpr buffer& operator=(const buffer& other) noexcept;
            constexpr buffer& operator=(buffer&& other) noexcept;

            // Concatenating
            buffer& operator+=(const buffer& other) noexcept;
            buffer operator+(const buffer& other) noexcept;

            // Element access
            constexpr reference operator[](const size_type index) noexcept;
            constexpr const_reference operator[](const size_type index) const noexcept;

            reference       front() noexcept;
            reference       back() noexcept;
            pointer         data() const noexcept { return data_ptr; }

            // Capacity
            void            reserve(size_type) noexcept;
            void            resize(size_type) noexcept;

            size_type       capacity() const noexcept { return current_size; }
            size_type       size() const noexcept { return element_count; }

            [[nodiscard]] bool empty() const noexcept { return (element_count == 0) || (data_ptr == nullptr); }

            // Iterators
            iterator        begin() noexcept;
            const_iterator  begin() const noexcept;
            iterator        end() noexcept;
            const_iterator  end() const noexcept;

            const_iterator  cbegin() const noexcept;
            const_iterator  cend() const noexcept;

            std::reverse_iterator<iterator>         rbegin() noexcept;
            std::reverse_iterator<const_iterator>   rbegin() const noexcept;
            std::reverse_iterator<iterator>         rend() noexcept;
            std::reverse_iterator<const_iterator>   rend() const noexcept;

            std::reverse_iterator<const_iterator>   crbegin() const noexcept;
            std::reverse_iterator<const_iterator>   crend() const noexcept;

            // Utility
            size_t          hash() const noexcept;
            size_t          seek(size_t target) const noexcept;

            // Read / Write
            std::string read_string(size_type length = 0) const noexcept;

            template <typename T>
            T read_strval() const noexcept;

            template <typename... T>
            size_t read(T... data) noexcept;

            template <typename... T>
            void write(T... data) noexcept;

        private:
            void grow(size_type amount) noexcept;

            pointer     data_ptr        = nullptr;
            size_type   element_count   = 0;
            size_type   current_size    = 0;

            mutable size_t read_pos = 0;
    };

    template <bool is_const> class buffer::iterator_type
    {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type        = std::byte;
            using difference_type   = std::ptrdiff_t;
            using pointer           = typename std::conditional<is_const, value_type const*, value_type*>::type;
            using reference         = typename std::conditional<is_const, value_type const&, value_type&>::type;

            constexpr iterator_type() noexcept = default;
            constexpr iterator_type(pointer ptr) noexcept : ptr(ptr) {}
            constexpr iterator_type(const iterator& other) noexcept : ptr(other.ptr) {}

            template <bool B1, bool B2>
            const iterator_type<B1> operator=(const iterator_type<B2>& other) const noexcept
            { iterator_type<B1> rval; rval.ptr = other.ptr; return rval; }

            iterator_type&  operator++()    noexcept { ptr++; return *this; }
            iterator_type   operator++(int) noexcept { iterator_type tmp(*this); ++(*this); return tmp; }
            iterator_type&  operator--()    noexcept { ptr--; return *this; }
            iterator_type   operator--(int) noexcept { iterator_type tmp(*this); --(*this); return tmp; }

            void            operator+=(const size_type n)           noexcept { ptr += n; }
            void            operator+=(const iterator_type& other)  noexcept { ptr += other.ptr; }
            iterator_type   operator+ (const size_type n)           noexcept { iterator_type tmp(*this); tmp +=n; return tmp; }
            size_type       operator+ (const iterator_type& other)  noexcept { return ptr + other.ptr; }

            void            operator-=(const size_type n)           noexcept { ptr -= n; }
            void            operator-=(const iterator_type& other)  noexcept { ptr -= other.ptr; }
            iterator_type   operator- (const size_type n)           noexcept { iterator_type tmp(*this); tmp -=n; return tmp; }
            size_type       operator- (const iterator_type& other)  noexcept { return ptr - other.ptr; }

            std::strong_ordering operator<=>(const iterator_type& other) noexcept { return ptr <=> other.ptr; }
            
            bool            operator!=(const iterator_type& other) noexcept { return ptr != other.ptr; };

            reference       operator[](const size_type index) noexcept { return *(ptr + index); }
            reference       operator*() noexcept { return *ptr; }
            pointer         operator->() noexcept { return *ptr; }

        private:
            pointer ptr = nullptr;

            friend class iterator_type<true>;
            friend class iterator_type<false>;
    };
}

namespace unorthodox
{
    constexpr buffer::buffer(const buffer& other) noexcept
    {
        reserve(other.element_count);
        element_count = other.element_count;
        nothrow_copy(other.begin(), other.end(), begin());
    }
    constexpr buffer::buffer(buffer&& other) noexcept
    {
        *this = std::move(other);
    }

    template <typename InputIt>
    constexpr buffer::buffer(InputIt first, InputIt last) noexcept
    {
        size_type new_count = 0;
        if constexpr(std::is_same<typename std::iterator_traits<InputIt>::iterator_category,
                                           std::random_access_iterator_tag>::value)
            new_count = last - first;
        else
            new_count = std::distance(first, last);

        reserve(new_count);
        nothrow_copy(first, last, begin());
        element_count = new_count;
    }

    buffer::~buffer()
    {
        if (data_ptr != nullptr)
            free(data_ptr);
    }

    constexpr buffer& buffer::operator=(buffer&& other) noexcept
    {
        data_ptr = other.data_ptr;
        element_count = other.element_count;
        current_size = other.current_size;

        other.data_ptr = nullptr;
        other.element_count = 0;
        other.current_size = 0;

        return *this;
    }

    // Conversions (string literals)
    buffer::buffer(const char* src) noexcept
    {
        size_type size = strlen(src);
        reserve(size);
        strcpy(reinterpret_cast<char*>(data_ptr), src);
        element_count = size;
    }
    
    buffer::buffer(char* src) noexcept
    {
        size_type size = strlen(src);
        reserve(size);
        strcpy(reinterpret_cast<char*>(data_ptr), src);
        element_count = size;
    }

    // Concatenating
    buffer& buffer::operator+=(const buffer& other) noexcept
    {
        reserve(capacity() + other.size());
        nothrow_copy(other.begin(), other.end(), end());
        element_count += other.size();

        return *this;
    }

    buffer buffer::operator+(const buffer& other) noexcept
    {
        buffer rval(*this);
        rval += other;
        return rval;
    }

    // Access
    constexpr buffer::reference buffer::operator[](const buffer::size_type index) noexcept { return data_ptr[index]; }
    constexpr buffer::const_reference buffer::operator[](const buffer::size_type index) const noexcept { return data_ptr[index]; }

    // Iterators
    buffer::iterator buffer::begin() noexcept { return iterator(&data_ptr[0]); }
    buffer::iterator buffer::end() noexcept { return iterator(&data_ptr[element_count]); }

    buffer::const_iterator buffer::begin() const noexcept { return iterator(&data_ptr[0]); }
    buffer::const_iterator buffer::end() const noexcept { return iterator(&data_ptr[element_count]); }
    
    buffer::const_iterator buffer::cbegin() const noexcept { return iterator(&data_ptr[0]); }
    buffer::const_iterator buffer::cend() const noexcept { return iterator(&data_ptr[element_count]); }

    // Memory
    void buffer::reserve(size_type new_size) noexcept
    {
        if (current_size >= new_size)
            return;

        void* new_ptr = realloc(data_ptr, new_size);

        if (new_ptr == nullptr)
            return;

        data_ptr = static_cast<pointer>(new_ptr);
        current_size = new_size;
    }

    void buffer::resize(size_type new_size) noexcept
    {
        if (current_size == new_size)
            return;
        if (current_size > new_size)
        {
            current_size = new_size;
            if (element_count > new_size)
                element_count = new_size;
        } else {
            void* new_ptr = realloc(data_ptr, new_size);
            if (new_ptr == nullptr)
                return;

            data_ptr = static_cast<pointer>(new_ptr);

            for (size_type i = current_size; i < new_size; ++i)
                ::new(data_ptr + i)(std::byte);
            current_size = new_size;
            element_count = new_size;
        }
    }

    // Read / Write
    inline std::string buffer::read_string(size_type length) const noexcept
    {
        std::string rval;

        if (length == 0)
            length = size() - read_pos;

        length = length + read_pos > size() ? size() - read_pos : length;

        for (size_t i = 0; i < length; ++i)
            rval += static_cast<char>(*(data_ptr + i));

        return rval;
    }

    // private functions
    // -----------------

    void buffer::grow(size_type amount) noexcept
    {
        reserve(std::max(capacity() * GROW_MULTIPLIER, capacity() + amount));
    }

}

#endif
