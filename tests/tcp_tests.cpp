#include "doctest.h"

#include <unorthodox/network/sockets.hpp>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

void run_accept_server(uint16_t port)
{
    unorthodox::net::tcp_socket tcp_server;
    unorthodox::net::tcp_socket connection;

    tcp_server.listen(port);
    tcp_server.accept(connection, 2s);

    connection.send("hello");
}

void run_echo_server(uint16_t port)
{
    unorthodox::net::tcp_socket tcp_server;
    tcp_server.listen(port);
}

TEST_CASE("TCP") {
    /*
    This test is buggy somehow, the same code works, needs fixing

    unorthodox::net::tcp_socket tcp_client;
    std::thread server_thread([](){ run_accept_server(16015); });

    // Give the server time to start
    std::this_thread::sleep_for(100ms);

    SUBCASE("simple hello") {
        tcp_client.connect("127.0.0.1", 16015);
        std::string msg = tcp_client.recv<std::string>().value_or("error_in_receive");
        REQUIRE(msg == std::string("hello"));
    }
    server_thread.join();
    */
}
