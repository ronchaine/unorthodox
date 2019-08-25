#ifndef UNORTHODOX_EMT_FIXED_POINT_HPP
#define UNORTHODOX_EMT_FIXED_POINT_HPP

#include <cstdint>
#include <type_traits>
#include <compare>

/*
 * TODO:
 *  needs to work with <random>
 *
 *  basic stuff:
 *  abs()
 *  fmod()
 *  remainder()
 *  max()
 *  min()
 *  exp()
 *  exp2()
 *  expm1()
 *  log()
 *  log10()
 *  log2()
 *  log1p()
 *  pow()
 *  sqrt()
 *  cbrt()
 *
 *  trigonometry:
 *  hypot()
 *  sin()
 *  cos()
 *  tan()
 *  asin()
 *  acos()
 *  atan()
 *  atan2()
 *
 *  sinh()
 *  cosh()
 *  tanh()
 *  asinh()
 *  acosh()
 *  atanh()
 */

namespace unorthodox
{
    template <unsigned int IntegerBits, unsigned int FractionBits, bool Signed = true>
    class fixed_point;

    template <unsigned int IntegerBits, unsigned int FractionBits>
    class unsigned_fixed_point : public fixed_point<IntegerBits, FractionBits, false> {};

    template <unsigned int bits>
    constexpr uint32_t bytesize_for_bits() noexcept
    {
        if (bits <= 8) return 1;
        else if (bits <= 16) return 2;
        else if (bits <= 32) return 4;
        else if (bits <= 64) return 8;
        else if (bits <= 128) return 16;

        return 0;
    }

    template <unsigned int Bytes>
    struct smallest_signed_integer_type { using type = void; };
    template <> struct smallest_signed_integer_type<1> { using type = int8_t; };
    template <> struct smallest_signed_integer_type<2> { using type = int16_t; };
    template <> struct smallest_signed_integer_type<4> { using type = int32_t; };
    template <> struct smallest_signed_integer_type<8> { using type = int64_t; };
    template <> struct smallest_signed_integer_type<16> { using type = __int128_t; };

    template <unsigned int Bytes>
    struct smallest_unsigned_integer_type { using type = void; };
    template <> struct smallest_unsigned_integer_type<1> { using type = uint8_t; };
    template <> struct smallest_unsigned_integer_type<2> { using type = uint16_t; };
    template <> struct smallest_unsigned_integer_type<4> { using type = uint32_t; };
    template <> struct smallest_unsigned_integer_type<8> { using type = uint64_t; };
    template <> struct smallest_unsigned_integer_type<16> { using type = __uint128_t; };

    template <typename T>
    struct is_fixed_point
    {
        constexpr static bool value = false;
        constexpr operator bool() { return value; }
    };
    
    template <unsigned int I, unsigned int F, bool S>
    struct is_fixed_point<fixed_point<I,F,S>>
    {
        constexpr static bool value = true;
        constexpr operator bool() { return value; }
    };

    template <unsigned int I, unsigned int F>
    struct is_fixed_point<unsigned_fixed_point<I,F>>
    {
        constexpr static bool value = true;
        constexpr operator bool() { return value; }
    };

    namespace detail
    {
        template <typename T>
        constexpr T mask_bits(size_t how_many)
        {
            T rval = ~0;
            rval = rval >> (how_many - (sizeof(T) * 8));
            rval = rval << (how_many - (sizeof(T) * 8));
            return rval;
        }

        template <typename T>
        constexpr T mask_high_bits(size_t how_many)
        {
            T rval = ~0;
            rval = rval >> ((sizeof(T) * 8) - how_many);
            rval = rval << ((sizeof(T) * 8) - how_many);
            return rval;
        }

        template <typename T>
        struct integer_overflow_struct
        { using type = void;};

        template <> struct integer_overflow_struct<int8_t> { using type = int16_t; };
        template <> struct integer_overflow_struct<int16_t> { using type = int32_t; };
        template <> struct integer_overflow_struct<int32_t> { using type = int64_t; };

        template <> struct integer_overflow_struct<uint8_t> { using type = uint16_t; };
        template <> struct integer_overflow_struct<uint16_t> { using type = uint32_t; };
        template <> struct integer_overflow_struct<uint32_t> { using type = uint64_t; };

        // ifdef these behind feature test macro or smth
        template <> struct integer_overflow_struct<int64_t> { using type = __int128; };
        template <> struct integer_overflow_struct<uint64_t> { using type = __uint128_t; };

        template <typename T>
        using integer_overflow_type = typename integer_overflow_struct<T>::type;
    }

    template <unsigned int IntegerBits, unsigned int FractionBits, bool Signed>
    class fixed_point
    {
        public:
            constexpr static uint32_t integer_bits = IntegerBits;
            constexpr static uint32_t fractional_bits = FractionBits;

            static_assert(not std::is_same<typename smallest_signed_integer_type<(IntegerBits + FractionBits) / 8>::type, void>::value);

            using underlying_type = typename std::conditional<Signed,
                                    typename smallest_signed_integer_type<(IntegerBits + FractionBits) / 8>::type,
                                    typename smallest_unsigned_integer_type<(IntegerBits + FractionBits) / 8>::type>::type;

            /*
             *  Construction / Destruction
             */
            constexpr fixed_point() noexcept = default;
            constexpr fixed_point(const fixed_point&) noexcept = default;
            constexpr fixed_point(fixed_point&&) noexcept = default;

            // conversion to
            template <typename T> requires(std::is_arithmetic<T>::value)
            constexpr fixed_point(T) noexcept;

            // conversion from
            template <typename T> requires(std::is_arithmetic<T>::value)
            constexpr operator T() const noexcept;

            template <uint32_t Int, uint32_t Frac, bool Sign>
            constexpr operator fixed_point<Int,Frac,Sign>() const noexcept;

            // comparison
            std::strong_ordering operator<=>(const fixed_point& rhs) const //= default;
            {
                return value <=> rhs.value;
            }

            /*
             * Assignment
             */
            constexpr fixed_point& operator=(fixed_point rhs) noexcept;

            /*
             * Arithmetic operations
             */
            template <typename T> constexpr fixed_point& operator +=(const T rhs) noexcept;
            template <typename T> constexpr fixed_point operator +(const T rhs) const noexcept;
            template <typename T> constexpr fixed_point& operator -=(const T rhs) noexcept;
            template <typename T> constexpr fixed_point operator -(const T rhs) const noexcept;

            template <typename T> constexpr fixed_point& operator *=(const T rhs) noexcept;
            template <typename T> constexpr fixed_point operator *(const T rhs) const noexcept;
            template <typename T> constexpr fixed_point& operator /=(const T rhs) noexcept;
            template <typename T> constexpr fixed_point operator /(const T rhs) const noexcept;

        private:
            friend struct std::hash<fixed_point>;
            
            template <uint32_t Int, uint32_t Frac, bool Sign>
            friend class fixed_point;

            underlying_type value = 0;
    };
}

namespace unorthodox
{
    // Assignment
    template <uint32_t Int, uint32_t Frac, bool Sign>
    constexpr fixed_point<Int,Frac,Sign>& fixed_point<Int,Frac,Sign>::operator=(fixed_point rhs) noexcept
    {
        value = rhs.value;
        return *this;
    }

    // Conversions
    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr fixed_point<Int,Frac,Sign>::fixed_point(T in) noexcept
    {
        if constexpr(std::is_integral<T>::value)
            value = in << fractional_bits;
        else if constexpr(std::is_floating_point<T>::value)
            value = in * (1 << fractional_bits);

        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "conversion from unknown arithmetic type");
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T> requires(std::is_arithmetic<T>::value)
    constexpr fixed_point<Int,Frac,Sign>::operator T() const noexcept
    {
        if constexpr(std::is_integral<T>::value)
            return static_cast<T>(value >> fractional_bits);
        else if constexpr(std::is_floating_point<T>::value)
            return static_cast<T>((T)value / (1 << fractional_bits));

        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "conversion to unknown arithmetic type");
    }

    template <uint32_t Int, uint32_t Frac, bool Sign>
    template <uint32_t Int2, uint32_t Frac2, bool Sign2>
    constexpr fixed_point<Int,Frac,Sign>::operator fixed_point<Int2,Frac2,Sign2>() const noexcept
    {
        fixed_point<Int2,Frac2,Sign2> rval;
        if constexpr(Frac2 > Frac)
            rval.value = (value << (Frac2 - Frac));
        else
            rval.value = (value >> (Frac - Frac2));

        return rval;
    }

    // Addition / substraction
    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign>& fixed_point<Int,Frac,Sign>::operator+=(const T rhs) noexcept
    {
        if constexpr(std::is_integral<T>::value)
            value += (rhs << fractional_bits);
        else if constexpr(std::is_floating_point<T>::value)
            value += rhs * (1 << fractional_bits);
        else if constexpr(std::is_same<typename std::decay<T>::type,
                                       typename std::decay<fixed_point<Int,Frac,Sign>>::type>::value)
            value += rhs.value;
        else
            *this += static_cast<fixed_point<Int,Frac,Sign>>(rhs);
        return *this;
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign>& fixed_point<Int,Frac,Sign>::operator-=(const T rhs) noexcept
    {
        if constexpr(std::is_integral<T>::value)
            value -= (rhs << fractional_bits);
        else if constexpr(std::is_floating_point<T>::value)
            value -= rhs * (1 << fractional_bits);
        else if constexpr(std::is_same<typename std::decay<T>::type,
                                       typename std::decay<fixed_point<Int,Frac,Sign>>::type>::value)
            value -= rhs.value;
        else
            *this -= static_cast<fixed_point<Int,Frac,Sign>>(rhs);
        return *this;
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign> fixed_point<Int,Frac,Sign>::operator+(const T rhs) const noexcept
    {
        return fixed_point<Int,Frac,Sign>(*this) += rhs;
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign> fixed_point<Int,Frac,Sign>::operator-(const T rhs) const noexcept
    {
        return fixed_point<Int,Frac,Sign>(*this) -= rhs;
    }

    // Multiplication / Division
    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign>& fixed_point<Int,Frac,Sign>::operator*=(const T rhs) noexcept
    {
        if constexpr (std::is_integral<T>::value)
            value *= rhs;
        else if constexpr (std::is_floating_point<T>::value)
            value *= rhs;
        else if constexpr (is_fixed_point<T>::value)
        {
            // fast way, if we have overflow type available
            if constexpr((fractional_bits == T::fractional_bits)
                      && (std::is_same<underlying_type, typename T::underlying_type>::value)
                      && (!std::is_same<detail::integer_overflow_type<underlying_type>, void>::value))
            {
                using overflow_type = detail::integer_overflow_type<underlying_type>;
                overflow_type tval = static_cast<overflow_type>(value) * static_cast<overflow_type>(rhs.value);
                tval >>= fractional_bits;
                value = tval;
            } else {
                // slower, but works without overflow types
                const underlying_type rv = static_cast<fixed_point<Int,Frac,Sign>>(rhs).value;

                const underlying_type lhs_int = value >> fractional_bits;
                const underlying_type lhs_frac = (value & ~mask_bits<underlying_type>(fractional_bits));

                const underlying_type rhs_int = rv >> fractional_bits;
                const underlying_type rhs_frac = (rv & ~mask_bits<underlying_type>(fractional_bits));

                const underlying_type x1 = lhs_int * rhs_int;
                const underlying_type x2 = lhs_int * rhs_frac;
                const underlying_type x3 = lhs_frac * rhs_int;
                const underlying_type x4 = lhs_frac * rhs_frac;

                value = (x1 << fractional_bits) + (x2 + x3) + (x4 >> fractional_bits);
            }
        }
        else
            *this *= static_cast<fixed_point<Int,Frac,Sign>>(rhs);
        return *this;
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign> fixed_point<Int,Frac,Sign>::operator*(const T rhs) const noexcept
    {
        return (fixed_point<Int,Frac,Sign>(*this)) *= rhs;
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign>& fixed_point<Int,Frac,Sign>::operator/=(const T rhs) noexcept
    {
        if constexpr (std::is_integral<T>::value)
            value /= rhs;
        else if constexpr (std::is_floating_point<T>::value)
            value /= rhs;
        else if constexpr (is_fixed_point<T>::value)
        {
            if constexpr((fractional_bits == T::fractional_bits)
                    && (std::is_same<underlying_type, typename T::underlying_type>::value)
                    && (!std::is_same<detail::integer_overflow_type<underlying_type>, void>::value))
            {
                using overflow_type = detail::integer_overflow_type<underlying_type>;
                overflow_type dividend = static_cast<overflow_type>(value);
                const overflow_type divisor = static_cast<overflow_type>(rhs.value);
                dividend <<= fractional_bits;
                value = dividend / divisor;
            }
        }
        else
            *this /= static_cast<fixed_point<Int,Frac,Sign>>(rhs);

        return *this;
    }

    template <uint32_t Int, uint32_t Frac, bool Sign> template <typename T>
    constexpr fixed_point<Int,Frac,Sign> fixed_point<Int,Frac,Sign>::operator/(const T rhs) const noexcept
    {
        return (fixed_point<Int,Frac,Sign>(*this)) /= rhs;
    }
}

namespace std
{
    template <uint32_t Int, uint32_t Frac, bool Sign>
    struct is_arithmetic<unorthodox::fixed_point<Int,Frac,Sign>>
    {
        constexpr static bool value = true;
        constexpr operator bool() { return value; }
    };

    template <uint32_t Int, uint32_t Frac, bool Sign>
    struct hash<unorthodox::fixed_point<Int,Frac,Sign>>
    {
        size_t operator()(const unorthodox::fixed_point<Int,Frac,Sign>& __v) const noexcept
        {
            return __v.value;
        }
    };
}
#endif
/*
 Licence

 Copyright (c) 2019 Jari Ronkainen

    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Permission is granted to anyone to use this software for any purpose, including
    commercial applications, and to alter it and redistribute it freely, subject to
    the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim
       that you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
