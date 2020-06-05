#ifndef UNORTHODOX_SHAPE_QUADRILATERAL_HPP
#define UNORTHODOX_SHAPE_QUADRILATERAL_HPP

#include "common.hpp"

namespace unorthodox::geometry
{
    template <typename PointType, std::size_t Dimension>
    struct quad : public shape
    {
        static_assert(Dimension == 2 or Dimension == 3);
        constexpr quad() noexcept = default;

        template <typename T, std::size_t N>
        constexpr quad(const T(&v0)[N], const T(&v1)[N], const T(&v2)[N], const T(&v3)[N]) noexcept
        {
            if constexpr(std::is_same_v<typename PointType::coordinate_system, cartesian_tag>)
            {
                if constexpr(N == 2) {
                    point[0] = {v0[0], v0[1]};
                    point[1] = {v1[0], v1[1]};
                    point[2] = {v2[0], v2[1]};
                    point[3] = {v3[0], v3[1]};
                }
                else if constexpr(N == 3) {
                    point[0] = {v0[0], v0[1], v0[2]};
                    point[1] = {v1[0], v1[1], v1[2]};
                    point[2] = {v2[0], v2[1], v2[2]};
                    point[3] = {v3[0], v3[1], v3[2]};
                }
            }
        }

        constexpr static size_t vertex_order[6] = { 0, 1, 3, 1, 2, 3 };

        typename std::conditional<Dimension == 2, cartesian_2d_point<PointType>, cartesian_3d_point<PointType>>::type
            point[4];
    };

    // deduction guide
    template <typename T, std::size_t N>
    quad(const T(&v0)[N], const T(&v1)[N], const T(&v2)[N], const T(&v3)[N]) -> quad<T, N>;
}

#endif
