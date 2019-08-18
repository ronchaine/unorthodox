#include "doctest.h"

#include <unorthodox/network/sockets.hpp>

void run_accept_server(uint16_t port)
{
    unorthodox::tcp_socket tcp_server;
    tcp_server.listen(port);
}

void run_echo_server(uint16_t port)
{
    unorthodox::tcp_socket tcp_server;
    tcp_server.listen(port);
}

TEST_CASE("TCP client-server") {
    run_echo_server(16015);
    SUBCASE("Hello world") {
    }
}
