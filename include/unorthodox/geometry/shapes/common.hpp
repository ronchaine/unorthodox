#ifndef UNORTHODOX_SHAPES_COMMON_HPP
#define UNORTHODOX_SHAPES_COMMON_HPP

#include <type_traits>

namespace unorthodox::geometry
{
    struct cartesian_tag {};
    struct polar_tag {};

    template <typename T>
    struct cartesian_3d_point
    {
        using value_type = T;
        using coordinate_system = cartesian_tag;

        constexpr static int dimension = 3;

        T x = 0;
        T y = 0;
        T z = 0;
    };

    template <typename T>
    struct cartesian_2d_point
    {
        using value_type = T;
        using coordinates = cartesian_tag;

        constexpr static int dimension = 2;

        T x = 0;
        T y = 0;
    };

    // deduction guides
    template <typename T> cartesian_3d_point(T, T, T) -> cartesian_3d_point<T>;
    template <typename T> cartesian_2d_point(T, T) -> cartesian_2d_point<T>;

    // shape base class for traits
    struct shape {};

    template <typename T> constexpr bool is_shape() noexcept { return std::is_base_of_v<shape, T>; }
}

#endif
