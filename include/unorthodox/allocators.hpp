#ifndef UNORTHODOX_ALLOCATORS_HPP
#define UNORTHODOX_ALLOCATORS_HPP

#include <new>
#include <limits>

namespace unorthodox::allocators
{
    template <typename T>
    struct nothrow_allocator
    {
        using value_type = T;
        using pointer = T*;

        using is_always_equal = std::true_type;

        constexpr nothrow_allocator() noexcept = default;

        template <class U>
        constexpr nothrow_allocator(const nothrow_allocator<U>&) {};

        [[nodiscard]] constexpr pointer allocate(size_t n) const noexcept;
        constexpr void deallocate(pointer p, size_t) const noexcept;
    };

    template <typename T, typename U>
    bool operator==(const nothrow_allocator<T>&, const nothrow_allocator<U>&) { return true; }

    template <typename T, typename U>
    bool operator!=(const nothrow_allocator<T>&, const nothrow_allocator<U>&) { return false; }
}

namespace unorthodox::allocators
{
    template <typename T>
    [[nodiscard]] constexpr T* nothrow_allocator<T>::allocate(size_t n) const noexcept
    {
        if (n > std::numeric_limits<size_t>::max() / sizeof(T))
            return nullptr;

        return new T[n];
    }
    
    template <typename T>
    constexpr void nothrow_allocator<T>::deallocate(pointer p, size_t) const noexcept
    {
        delete[] p;
    }
}

#endif
