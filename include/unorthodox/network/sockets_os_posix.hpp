#ifndef UNORTHODOX_NETWORK_SOCKETS_PLATFORM_HPP
#define UNORTHODOX_NETWORK_SOCKETS_PLATFORM_HPP

#include <iostream>
#include <unorthodox/error_codes.hpp>
#include <chrono>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>

#if defined(UNORTHODOX_OS_LINUX)
# include <sys/epoll.h>
#elif defined(UNORTHODOX_OS_BSD)
# include <sys/event.h>
#endif

namespace unorthodox::net
{
    #if defined(UNORTHODOX_OS_LINUX)
    using platform_event_type = epoll_event;
    #elif defined(UNORTHODOX_OS_BSD)
    using platform_event_type = kevent;
    #endif
}

namespace unorthodox::net::os
{
    using socket_type = int;

    constexpr static socket_type uninitialised_socket_value   = 0;
    constexpr static socket_type disabled_socket_value        = -2;
    constexpr static socket_type socket_error                 = -1;

    class socket_specifics
    {
        protected:
            socket_specifics() = default;
            socket_specifics(socket_specifics&&) = default;

            error_code platform_listen_socket(uint16_t port, int backlog_size, os::socket_type& sock, int socktype = AF_INET) noexcept;
            void stop_listening() noexcept { ::close(listen_fd); listen_fd = uninitialised_socket_value; }

            int block_until_listen_event(platform_event_type* output, uint32_t max_events, std::chrono::milliseconds count) noexcept;

            socket_type platform_get_socket_from_event(platform_event_type& event) const noexcept;

            //event_count = epoll_wait(listen_fd, target, max_events, timeout.count());
        private:
            socket_type listen_fd = uninitialised_socket_value;

            #if defined(UNORTHODOX_OS_BSD)
            constexpr static size_t ipv4_index = 0;
            constexpr static size_t ipv6_index = 1;
            uint8_t listen_slots = 0;
            kevent listen_event[2];
            #endif
    };

    error_code socket_specifics::platform_listen_socket(uint16_t port, int backlog_size, os::socket_type& sock, int socktype) noexcept
    {
        (void) port;
        (void) socktype;

        
        if (listen_fd == uninitialised_socket_value)
        {
            #if defined(UNORTHODOX_OS_LINUX)
            listen_fd = epoll_create1(0);
            #elif defined(UNORTHODOX_OS_BSD)
            listen_fd = kqueue();
            #endif
        }
        if (listen_fd == -1)
        {
            stop_listening();
            return error_code(error_domain::network_error, error_value::poll_error);
        }

        if (::listen(sock, backlog_size) == -1)
            return error_code(error_domain::network_error, error_value::from_errno());

        #if defined(UNORTHODOX_OS_LINUX)
        epoll_event event{};
        event.events = EPOLLIN;
        event.data.fd = sock;
        if (epoll_ctl(listen_fd, EPOLL_CTL_ADD, sock, &event))
            return error_code(error_domain::network_error, error_value::poll_error);

        #elif defined(UNORTHODOX_OS_BSD)
        if (socktype == AF_INET)
        {
            EV_SET(&listen_event[ipv4_index], sock, EVFILT_READ, EV_ADD, 0, 0, 0);
            listen_slots &= 1 << ipv4_index;
        }
        else if (socktype == AF_INET6)
        {
            EV_SET(&listen_event[ipv6_index], sock, EVFILT_READ, EV_ADD, 0, 0, 0);
            listen_slots &= 1 << ipv6_index;
        }
        #endif

        return error_code::success;
    }

    int socket_specifics::block_until_listen_event(platform_event_type* output, uint32_t max_events, std::chrono::milliseconds timeout) noexcept
    {
        #if defined(UNORTHODOX_OS_LINUX)
            if (timeout.count() < 0)
                return epoll_wait(listen_fd, output, max_events, -1);
            else
                return epoll_wait(listen_fd, output, max_events, timeout.count());
        #else
            // TODO: I wrote this blind, test, refactor, etc.
            timespec timeout_ts = { std::chrono::duration_cast<std::chrono::seconds>(timeout).count, 
                                    std::chrono::duration_cast<std::chrono::nanoseconds>
                                        (timeout - std::chrono::duration_cast<std::chrono::seconds>(timeout).count()); }

            int count = 0;
            int start = 0;
            if (!(listen_slots & ipv4_index))
                start++;
            else
                count++;

            if (listen_slots & ipv6_index)
                count++;

            if (timeout.count() < 0)
                return kevent(listen_fd, &listen_event[start], count, output, max_events, nullptr);
            else
                return kevent(listen_fd, &listen_event[start], count, output, max_events, &timeout_ts);
        #endif
    }

    socket_type socket_specifics::platform_get_socket_from_event(platform_event_type& event) const noexcept
    {        
        #if defined(UNORTHODOX_OS_LINUX)
        return event.data.fd;
        #elif defined(UNORTHODOX_OS_BSD)
        return event.ident;
        #endif
    }
}

#endif
