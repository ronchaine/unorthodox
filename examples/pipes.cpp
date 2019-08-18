#include <unorthodox/unix/pipe.hpp>
#include <iostream>

int main()
{
    auto child = unorthodox::unix::process("ls");

    if (child)
    {
        std::cout << "Process dead\n";
    }

    return 0;
}
