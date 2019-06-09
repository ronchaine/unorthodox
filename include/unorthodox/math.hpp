#ifndef UNORTHODOX_MATH_HPP
#define UNORTHODOX_MATH_HPP

#include <limits>

namespace unorthodox
{
    namespace detail
    {
        template <size_t Size> struct int_of_size_s { using type = void; };

        template <> struct int_of_size_s<1>  { using type = int8_t; };
        template <> struct int_of_size_s<2>  { using type = int16_t; };
        template <> struct int_of_size_s<4>  { using type = int32_t; };
        template <> struct int_of_size_s<8>  { using type = int64_t; };
        template <> struct int_of_size_s<16> { using type = __int128_t; };
        
        template <size_t Size> struct uint_of_size_s { using type = void; };

        template <> struct uint_of_size_s<1>  { using type = uint8_t; };
        template <> struct uint_of_size_s<2>  { using type = uint16_t; };
        template <> struct uint_of_size_s<4>  { using type = uint32_t; };
        template <> struct uint_of_size_s<8>  { using type = uint64_t; };
        template <> struct uint_of_size_s<16> { using type = __uint128_t; };

        template <typename T> requires(std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559)
        struct fp_info
        {
            static_assert(sizeof(T) == 8 || sizeof(T) == 4, "Only supports iec559 float/double types");
            using type = T;

            constexpr static int fraction_bits = sizeof(T) == 8 ? 52 : 23;
            constexpr static int exponent_bits = sizeof(T) == 8 ? 11 : 8;
            constexpr static int exponent_mask = sizeof(T) == 8 ? 0x7ff : 0xff;
//            constexpr static int exponent_magic = sizeof(T) == 8 ? 0x3ff + 52 : 0x7f + 23;
            constexpr static int exponent_magic = sizeof(T) == 8 ? 0x3ff: 0x7f;

            constexpr static int total_bits_minus_one = sizeof(T) * 8 - 1;
        };
    }

    template <size_t Size> using int_of_size = detail::int_of_size_s<Size>;
    template <typename T> using match_int_size = int_of_size<sizeof(T)>;

    template <size_t Size> using uint_of_size = detail::uint_of_size_s<Size>;
    template <typename T> using match_uint_size = uint_of_size<sizeof(T)>;

    template <typename T> requires(std::is_floating_point<T>::value)
    constexpr inline T round_iec559(T value)
    {
        union { T f; match_uint_size<T> i; } u{value};
        int e = (u.i >> detail::fp_info<T>::fraction_bits) & detail::fp_info<T>::exponent_mask;

        if (e >= detail::fp_info<T>::exponent_magic + detail::fp_info<T>::fraction_bits)
            return value;

        if (u.i >= detail::fp_info<T>::total_bits_minus_one)
            value = -value;

        if (e < detail::fp_info<T>::exponent_magic - 1)
            return 0 * u.f;

//        T fract = fractii;

        if (u.i >= detail::fp_info<T>::total_bits_minus_one)
            value = -value;
        return fract;
/*
double round(double x)
{
	double_t y;

	y = x + toint - toint - x;
	if (y > 0.5)
		y = y + x - 1;
	else if (y <= -0.5)
		y = y + x + 1;
	else
		y = y + x;
	if (u.i >> 63) (d)
	if (u.i >> 31) (f)
		y = -y;
	return y;
}
*/
    }
    
    template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr inline T round_icast(T value)
    {
        return static_cast<T>(static_cast<int>(value + 0.5));
    }

    template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr inline T round(T value)
    {
        if constexpr (std::is_floating_point<T>::value)
        {
            if constexpr (!std::numeric_limits<T>::is_iec559)
                return round_icast(std::forward<T>(value));
            else
                return round_iec559(std::forward<T>(value));
        } else {
            return value;
        }
    }

    template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr inline T abs(const T& value) noexcept
    {
        return value < 0 ? -value : value;
    }

    template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr inline T modulo(const T a, const T b) noexcept
    {
        if constexpr (std::is_floating_point<T>::value)
        {
            return (a - (int(a / b) * b));
        } else {
            return a % b;
        }
    }
}

#endif
