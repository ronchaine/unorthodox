#include <iostream>

#include <unorthodox/buffer.hpp>
#include <unorthodox/file.hpp>

int main()
{
    std::cout << std::boolalpha << unorthodox::big_endian_system() << "\n";
    std::cout << std::is_same<std::byte, unorthodox::buffer::value_type>::value << "\n";

    std::cout << sizeof(unorthodox::file) << "\n";

    auto file = unorthodox::file::open("/etc/ld.so.conf");

    if (file)
        std::cout << "File open\n";
    else
        std::cout << "Can't open file: " << unorthodox::error_string(file.error()) << "\n";
}
