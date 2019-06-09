#ifndef UNORTHODOX_SHAPE_TRIANGLE_HPP
#define UNORTHODOX_SHAPE_TRIANGLE_HPP

#include "common.hpp"

namespace unorthodox::geometry
{
    template <typename PointType, std::size_t Dimension>
    class triangle : public shape
    {
        static_assert(Dimension == 2 or Dimension == 3);
        public:
            constexpr triangle() noexcept = default;

            template <typename T, std::size_t N>
            constexpr triangle(const T(&v0)[N], const T(&v1)[N], const T(&v2)[N]) noexcept
            {
                if constexpr(std::is_same_v<typename cartesian_3d_point<T>::coordinate_system, cartesian_tag>)
                {
                    if constexpr(N == 2) {
                        point[0] = {v0[0], v0[1]};
                        point[1] = {v1[0], v1[1]};
                        point[2] = {v2[0], v2[1]};
                    }
                    if constexpr(N == 3) {
                        point[0] = {v0[0], v0[1], v0[2]};
                        point[1] = {v1[0], v1[1], v1[2]};
                        point[2] = {v2[0], v2[1], v2[2]};
                    }
                }
            }

            constexpr static size_t vertex_order[3] = { 0, 1, 2 };

        private:
            typename std::conditional<Dimension == 2, cartesian_2d_point<PointType>, cartesian_3d_point<PointType>>::type
                point[3];
    };

    // deduction guide
    template <typename T, std::size_t N>
    triangle(const T(&v0)[N], const T(&v1)[N], const T(&v2)[N]) -> triangle<T, N>;
}
#endif
