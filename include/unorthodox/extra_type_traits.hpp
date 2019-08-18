#ifndef UNORTHODOX_EXTRA_TYPE_TRAITS_HPP
#define UNORTHODOX_EXTRA_TYPE_TRAITS_HPP

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

        { t.begin() } -> typename T::iterator;
        { t.end() } -> typename T::iterator;
        { t.size() };
    };

    template <typename T>                   constexpr bool is_stl_like_container() { return false; };
    template <stl_compatible_container T>   constexpr bool is_stl_like_container() { return true; };
}

#endif
