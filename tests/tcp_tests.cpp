#include "doctest.h"

#include <unorthodox/network/sockets.hpp>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

void run_accept_server(uint16_t port)
{
    unorthodox::tcp_socket tcp_server;
    unorthodox::tcp_socket connection;

    tcp_server.listen(port);
    tcp_server.accept(connection, 2s);

    connection.send("hello");
}

void run_echo_server(uint16_t port)
{
    unorthodox::tcp_socket tcp_server;
    tcp_server.listen(port);
}

TEST_CASE("TCP") {
    unorthodox::tcp_socket tcp_client;
    unorthodox::buffer recv_buf;
    std::thread server_thread([](){ run_accept_server(16015); });

    // Give the server time to start
    std::this_thread::sleep_for(10ms);

    SUBCASE("hello") {
        tcp_client.connect("127.0.0.1", 16015);
        recv_buf = tcp_client.recv();
        REQUIRE(recv_buf.size() == 5);
        REQUIRE(static_cast<char>(recv_buf[0]) == 'h');
        REQUIRE(static_cast<char>(recv_buf[1]) == 'e');
        REQUIRE(static_cast<char>(recv_buf[2]) == 'l');
        REQUIRE(static_cast<char>(recv_buf[3]) == 'l');
        REQUIRE(static_cast<char>(recv_buf[4]) == 'o');
    }
    server_thread.join();
}
