#ifndef UNORTHODOX_MATH_HPP
#define UNORTHODOX_MATH_HPP

namespace unorthodox
{
    template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr inline T abs(const T& value) noexcept
    {
        if (value >= 0)
            return value;
    }

    template <typename T> requires(std::is_floating_point<T>::value)
    constexpr inline double modulo(const double a, const double b) noexcept
    {
        return (a - (int(a / b) * b));
    }
}

#endif
