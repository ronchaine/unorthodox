#ifndef UNORTHODOX_EMT_FIXED_POINT_HPP
#define UNORTHODOX_EMT_FIXED_POINT_HPP

#include <cstdint>
#include <type_traits>
#include <compare>

/*
 * TODO:
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
    template <unsigned int WholeBits, unsigned int FractionBits, bool Signed = true>
    class fixed_point;

    template <unsigned int WholeBits, unsigned int FractionBits>
    class unsigned_fixed_point : public fixed_point<WholeBits, FractionBits, false> {};

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

            using underlying_type = std::conditional<Signed,
                                    smallest_signed_integer_type<IntegerBits + FractionBits>,
                                    smallest_unsigned_integer_type<IntegerBits + FractionBits>>;

            /*
             *  Construction / Destruction
             */
            constexpr fixed_point() noexcept = default;
            constexpr fixed_point(const fixed_point&) noexcept = default;
            constexpr fixed_point(fixed_point&&) noexcept = default;

            // conversion to
            template <typename T>
            constexpr fixed_point(T) noexcept;

            // conversion from
            template <typename T> requires(std::is_arithmetic<T>::value)
            constexpr operator T() const noexcept;

            template <uint32_t Int, uint32_t Frac, bool Sign>
            constexpr operator fixed_point<Int,Frac,Sign>() const noexcept;

            // comparison
            std::strong_ordering operator<=>(const fixed_point& rhs) const = default;

            /*
             * Assignment
             */
            constexpr fixed_point& operator=(fixed_point&& rhs) noexcept;
            constexpr fixed_point& operator=(const fixed_point& rhs) noexcept;

            /*
             * Arithmetic operations
             */
            template <typename T> constexpr fixed_point& operator +=(const T& rhs) noexcept;
            template <typename T> constexpr fixed_point operator +(const T& rhs) noexcept;
            template <typename T> constexpr fixed_point& operator -=(const T& rhs) noexcept;
            template <typename T> constexpr fixed_point operator -(const T& rhs) noexcept;

            template <typename T> constexpr fixed_point& operator *=(const T& rhs) noexcept;
            template <typename T> constexpr fixed_point operator *(const T& rhs) noexcept;
            template <typename T> constexpr fixed_point& operator /=(const T& rhs) noexcept;
            template <typename T> constexpr fixed_point operator /(const T& rhs) noexcept;

        private:
            friend struct std::hash<fixed_point>;
            
            template <uint32_t Int, uint32_t Frac, bool Sign>
            friend class fixed_point;

            underlying_type value = 0;
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
