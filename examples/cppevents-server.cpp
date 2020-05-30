#define HAS_CPPEVENTS

#include <sys/signal.h>

#include <unorthodox/network/sockets.hpp>
#include <cppevents/signal.hpp>
#include <vector>

static volatile bool exit_requested = false;

int main()
{
    unorthodox::net::tcp_socket server;
    std::vector<unorthodox::net::tcp_socket> clients;

    server.listen(16015);

    cppevents::add_source<cppevents::signal>(SIGINT);
    cppevents::add_source<cppevents::socket_listener>(server);

    cppevents::on_event<cppevents::signal_event>([&](cppevents::event& raw) {
        auto ev = event_cast<cppevents::signal_event>(raw);

        if (ev.signal_no == SIGINT)
        {
            std::cout << "**SIGINT received, quitting...\n";
            exit_requested = true;
        }
    });

    cppevents::on_event<cppevents::network_event>([&](cppevents::event& raw){
        auto ev = event_cast<cppevents::network_event>(raw);

        if (ev.type == cppevents::network_event::new_connection)
        {
            std::cout << "new connection from " << ev.peer_address << ", " << ev.peer_port << ", socket fd: " << ev.sock_handle << "\n";

            clients.emplace_back(unorthodox::net::tcp_socket(ev));
            cppevents::add_source<cppevents::socket_io_event>(clients.back());
            clients.back().send("hello!\n");
        }
        else if (ev.type == cppevents::network_event::socket_ready)
        {
            std::cout << "message from " << ev.sock_handle << "(" << ev.peer_address << "," << ev.peer_port << ")\n";

            auto client = unorthodox::net::tcp_socket::get_socket_from_event(ev);
            std::string data = client.recv<std::string>().value_or("error");

            std::cout << data;
        }
        else if (ev.type == cppevents::network_event::connection_closed)
        {
            std::cout << ev.peer_address << ":" << ev.peer_port << " closed connection\n";
        }
    });

    std::cout << "CTRL+C to quit\n";
    while(!exit_requested)
        cppevents::wait();
}

