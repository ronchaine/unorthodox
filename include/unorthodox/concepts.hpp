#ifndef UNORTHODOX_CONCEPTS_HPP
#define UNORTHODOX_CONCEPTS_HPP

#include <type_traits>
#include <cstddef>

namespace unorthodox
{
    template <typename T>
    concept not_void = not std::is_same_v<T, void>;

    template <typename T>
    concept iterable_type = requires(T t)
    {
        { t.begin() };
        { t.end() };

        { *t.begin() } -> not_void;
        { *t.end() } -> not_void;
        
        { *t.begin()++ };
        { *t.end()++ };
    };

    template <typename T> concept std_compatible_allocator = requires(T t)
    {
        typename T::value_type;

        { t.allocate(size_t{}) };
        { t.deallocate };
    };

    template <typename T> concept reallocable_allocator = std_compatible_allocator<T> && requires(T t)
    {
        { t.reallocate(size_t{}) };
    };
}

#endif
