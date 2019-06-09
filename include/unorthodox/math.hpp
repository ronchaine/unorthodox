#ifndef UNORTHODOX_MATH_HPP
#define UNORTHODOX_MATH_HPP

namespace unorthodox
{
    template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr inline T abs(const T& value) noexcept
    {
        return value < 0 ? -value : value;
    }

    template <typename T> requires(std::is_floating_point<T>::value)
    constexpr inline T modulo(const T a, const T b) noexcept
    {
        return (a - (int(a / b) * b));
    }
}

#endif
