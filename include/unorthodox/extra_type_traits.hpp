#ifndef UNORTHODOX_EXTRA_TYPE_TRAITS_HPP
#define UNORTHODOX_EXTRA_TYPE_TRAITS_HPP

#include "concepts.hpp"

namespace unorthodox
{
    // Doesn't require all STL functions to be present
    template <typename T> concept stl_compatible_container = requires(T t)
    {
        typename T::value_type;
        typename T::reference;
        typename T::const_reference;

        typename T::iterator;
        typename T::const_iterator;

        typename T::difference_type;
        typename T::size_type;

        { t.begin() };
        { t.end() };
        /*
        { t.begin() } -> std::convertible_to<T::iterator>;
        { t.end() } -> std::convertible_to<T::iterator>;
        */
        { t.size() };
    };

    template <typename T>                   constexpr bool is_stl_like_container() { return false; };
    template <stl_compatible_container T>   constexpr bool is_stl_like_container() { return true; };

    template <typename T>
    constexpr static bool allocator_can_realloc() noexcept { return false; }

    template <typename T> requires reallocable_allocator<T>
    constexpr static bool allocator_can_realloc() noexcept { return true; }

    template <typename T>
    constexpr static bool is_string_literal()
    {
        return std::is_same_v<T, std::add_lvalue_reference_t<const char[std::extent_v<std::remove_reference_t<T>>]>>;
    }
}

#endif
