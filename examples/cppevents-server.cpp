#define HAS_CPPEVENTS
#include <sys/signal.h>

#include <unorthodox/network/sockets.hpp>
#include <cppevents/signal.hpp>
#include <vector>

// create a value for ending the wait loop
static volatile bool exit_requested = false;

int main()
{
    // we need one socket for the server and one socket for each
    // client
    unorthodox::net::tcp_socket server;
    std::vector<unorthodox::net::tcp_socket> clients;

    // set server to listen on port 16015
    server.listen(16015);

    // we want to capture SIGINT signal for exiting gracefully
    // when CTRL+C is pressed
    cppevents::add_source<cppevents::signal>(SIGINT);

    // and we want to receive the server accept events too
    cppevents::add_source<cppevents::socket_listener>(server);


    // define the signal handler for when we get SIGINT
    cppevents::on_event<cppevents::signal_event>([&](cppevents::event& raw) {
        // we need to cast the event to right type first
        auto ev = event_cast<cppevents::signal_event>(raw);

        if (ev.signal_no == SIGINT)
        {
            std::cout << "**SIGINT received, quitting...\n";
            exit_requested = true;
        }
    });

    // define handler for network events
    cppevents::on_event<cppevents::network_event>([&](cppevents::event& raw){
        // again, cast the event to the right type
        auto ev = event_cast<cppevents::network_event>(raw);

        // if we receive new connection, print the address of the peer to
        // stdout and then add the client to the list of clients
        if (ev.type == cppevents::network_event::new_connection)
        {
            std::cout << "new connection from " << ev.peer_address << ", " << ev.peer_port << ", socket fd: " << ev.sock_handle << "\n";

            clients.emplace_back(unorthodox::net::tcp_socket(ev));
            cppevents::add_source<cppevents::socket_io_event>(clients.back());

            // also send the client some text so they know they connected
            clients.back().send("hello!\n");
        }

        // the event can also tell us if a socket is ready to read some data
        else if (ev.type == cppevents::network_event::socket_ready)
        {
            // we can tell who is going to be sending the message already
            std::cout << "message from " << ev.sock_handle << "(" << ev.peer_address << "," << ev.peer_port << ")\n";

            // then we get non-owning socket from the event (it doesn't close()
            // when its destructor is ran)
            auto client = unorthodox::net::tcp_socket::get_socket_from_event(ev);

            // Receive data from the client, recv returns tl::expected<T, error_code>
            // so calling value_or("error") means that if there was an error receiving,
            // then we use the "or" part and set the string to "error"
            std::string data = client.recv<std::string>().value_or("error");

            // and print the string we got
            std::cout << data;
        }

        // just say if someone closed the connection
        else if (ev.type == cppevents::network_event::connection_closed)
        {
            std::cout << ev.peer_address << ":" << ev.peer_port << " closed connection\n";
        }
    });

    std::cout << "CTRL+C to quit\n";
    while(!exit_requested)
        cppevents::wait();
}

