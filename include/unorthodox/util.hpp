#ifndef UNORTHODOX_UTILITY_HPP
#define UNORTHODOX_UTILITY_HPP

#include <cstdint>
#include <cstring>
#include <memory>

namespace unorthodox
{
    struct empty {};

    constexpr inline bool big_endian_system() noexcept
    {
        uint16_t check = 0xbe1e;
        return (uint8_t)*(&check) == 0xbe;
    }

    template <typename T>
    constexpr T swap_endianness(T value) noexcept
    {
        union
        {
            T value;
            uint8_t value_u8[sizeof(T)];
        } src, dst;

        src.value = std::forward<T>(value);
        
        for (std::size_t i = 0; i < sizeof(T); ++i)
            dst.value_u8[i] = src.value_u8[sizeof(T)-1-i];

        return dst.value;
    }

    template <typename InputIt, typename OutputIt>
    inline constexpr OutputIt nothrow_copy(InputIt first, InputIt last, OutputIt dest) noexcept
    {
        if constexpr(std::is_pointer<OutputIt>::value)
        {
            if ((first == nullptr) || (dest == nullptr))
                return nullptr;
        }

        if (std::is_constant_evaluated())
        {
            while (first != last)
                *dest++ = *first++;
            return dest;
        } else {
            if constexpr(!std::is_pointer<InputIt>::value)
                static_assert(std::is_nothrow_copy_assignable<typename InputIt::value_type>::value);

            if constexpr(std::is_same_v<typename std::remove_const<InputIt>::type, OutputIt>
                         && std::is_trivially_copy_assignable<OutputIt>::value)
            {
                const size_t e_count = static_cast<size_t>(last - first);
                if (e_count)
                    std::memmove((void*)&(*dest), (void*)&(*first), e_count * sizeof(typename OutputIt::value_type));

                return dest + e_count;
            } else {
                for (; first != last; ++first, (void) ++dest)
                    *dest = *first;

                return dest;
            }
        }
    }
}

#endif
