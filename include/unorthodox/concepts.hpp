#ifndef UNORTHODOX_CONCEPTS_HPP
#define UNORTHODOX_CONCEPTS_HPP

#include <type_traits>

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
}

#endif
