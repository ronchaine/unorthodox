#ifndef UNORTHODOX_EMT_FIXED_POINT_HPP
#define UNORTHODOX_EMT_FIXED_POINT_HPP

#include <cstdint>
#include <type_traits>

namespace unorthodox
{
    template <unsigned int ScalingFactor, typename BaseType = int32_t>
    class fixed_point;

    template <typename T>
    struct is_fixed_point
    {
        constexpr static bool value = false;
        constexpr operator bool() { return value; }
    };
    
    template <unsigned int P, typename U>
    struct is_fixed_point<fixed_point<P,U>>
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

    template <unsigned int Precision, typename BaseType>
    class fixed_point
    {
        public:
            constexpr static uint32_t integer_bits = sizeof(BaseType) - Precision;
            constexpr static uint32_t fractional_bits = Precision;
            using underlying_type = BaseType;

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

            template <uint32_t P, typename U>
            constexpr operator fixed_point<P,U>() const noexcept;

            // comparison
            //std::strong_ordering operator<=>(const fixed_point& rhs) const = default;
            template <typename T> constexpr bool operator==(const T& rhs) noexcept
            {
                return value == static_cast<fixed_point>(rhs).value;
            }

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
            
            template <uint32_t P, typename U>
            friend class fixed_point;

            BaseType value = 0;
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
