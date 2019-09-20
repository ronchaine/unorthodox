#include <type_traits>
#include <cstdint>

#include <array>
#include <vector>

namespace std
{
    enum class byte : unsigned char {};
}

namespace unorthodox
{
    using error_code = uint32_t;

    namespace encoding
    {
        struct utf8
        {
            using codepoint_type = uint32_t;

            constexpr static bool fixed_width = false;

            template <std::size_t Size>
            constexpr static error_code encode(const char(&str)[Size], std::array<std::byte, Size * sizeof(codepoint_type)>& target)
            {
                target = str;
                return 0;
            }
        };
    }

    template <typename Encoding, std::size_t N>
    class string
    {
        constexpr static std::size_t maximum_stack_alloc_size = 256;
        public:
            using encoding_type = Encoding;
            using value_type    = typename Encoding::codepoint_type;

            using storage_type = typename std::conditional<
                                    N * sizeof(value_type) <= maximum_stack_alloc_size,
                                    std::array<std::byte, N * sizeof(value_type)>,
                                    std::vector<std::byte>>::type;

            // Constructors
            constexpr string() noexcept {}
            constexpr string(const char(&str)[N]) noexcept; // from string literal

            constexpr string(const string& other) noexcept;
            constexpr string(string&& other) noexcept;

            constexpr value_type operator[](const std::size_t index) noexcept;

        private:
            storage_type stored_data;
    };

    // Deduction guides default to utf8 encoding with string literals
    template <std::size_t N> string(const char(&str)[N]) -> string<encoding::utf8, N>;


    template <typename Enc, std::size_t N>
    constexpr string<Enc,N>::string(const char(&str)[N]) noexcept
    {
        if constexpr (std::is_same<storage_type, std::vector<char>>::value)
            encoding_type::encode(str, reinterpret_cast<char&>(*(&stored_data[0])));
        else
            encoding_type::encode(str, stored_data);
    }

    template <typename Enc, std::size_t N>
    constexpr string<Enc,N>::string(const string& other) noexcept
    {
    }

    template <typename Enc, std::size_t N>
    constexpr string<Enc,N>::string(string&& other) noexcept
    {
    }

    template <typename Enc, std::size_t N>
    constexpr typename string<Enc,N>::value_type string<Enc,N>::operator[](const std::size_t index) noexcept
    {
        if (index > stored_data.size())
            return 0xFFFD; //object replacement character
        return static_cast<value_type>(stored_data[index]);
    }
}


int main()
{
    auto string = unorthodox::string("hello world");
    static_assert(string[4] == 'o', "");
}
