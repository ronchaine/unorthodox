/*
 * Unorthodox network - sockets support
 *
 * TODO:
 *  Windows support
 *  BSD support
 *
 *  generic unix / select fallback
 */

#ifndef UNORTHODOX_NETWORK_SOCKETS_HPP
#define UNORTHODOX_NETWORK_SOCKETS_HPP

#include <unorthodox/buffer.hpp>
#include <unorthodox/error_codes.hpp>
#include <unorthodox/extra_type_traits.hpp>

#if defined(HAS_CPPEVENTS)
#include <cppevents/network.hpp>
#include <unordered_set>
#include <unordered_map>
#endif

#if defined(__linux__) || defined(__linux)
#include <sys/epoll.h>
#include <sys/socket.h>
#define EPOLL_MAX_EVENTS 32
#elif defined(__bsdi__) || defined(__DragonFly__)
#include <sys/event.h>
#include <sys/socket.h>
#warning No full BSD implementation (yet)
#elif (defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/event.h>
#include <sys/socket.h>
#warning No full BSD implementation (yet)
#elif (defined(_WIN32))
#error No windows implementation (yet)
#endif

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <cstring>

namespace unorthodox
{
    namespace helpers
    {
        inline void* get_in_addr(struct sockaddr* sa)
        {
            if (sa->sa_family == AF_INET)
                return &(((struct sockaddr_in*)sa)->sin_addr);
            else
                return &(((struct sockaddr_in6*)sa)->sin6_addr);
        }

        inline int deduce_protocol_from_address(const std::string& s)
        {
            addrinfo* result;
            getaddrinfo(s.c_str(), nullptr, nullptr, &result);
            int rval = result->ai_family;
            freeaddrinfo(result);
            return rval;
        }
    }

    struct socket_config
    {
        bool ipv6_enabled   : 1 = true;
        bool ipv4_enabled   : 1 = true;

        bool reuse_address  : 1 = true;
        bool reuse_port     : 1 = false;
    };

    class toggle
    {
        public:
            using toggle_function_type = std::function<void(bool&, bool)>;

            toggle(bool init) : state(init) {}
            operator bool() { return state; }

            bool& operator=(bool new_state)
            {
                if (toggle_func == nullptr)
                    state = new_state;
                else
                    toggle_func(state, new_state);

                return state;
            }

            void on_toggle(toggle_function_type new_func) noexcept { toggle_func = new_func; }

        private:
            toggle_function_type toggle_func = nullptr;
            bool state = false;
    };

    class tcp_socket_details
    {
        public:
            constexpr static int    domain  = PF_INET;
            constexpr static int    type    = SOCK_STREAM;
            constexpr static bool   secure  = false;

            toggle tcp_nodelay = false;
            toggle tcp_quickack = false;

            tcp_socket_details()
            {
                tcp_nodelay.on_toggle(nullptr);
                tcp_quickack.on_toggle(nullptr);
            }
    };

    struct udp_socket_details
    {
        constexpr static int    domain  = PF_INET;
        constexpr static int    type    = SOCK_DGRAM;
        constexpr static bool   secure  = false;
    };

    #if defined(UNORTHODOX_SSL_PROVIDER)
    struct ssl_socket_details
    {
        constexpr static int    domain  = PF_INET;
        constexpr static int    type    = SOCK_STREAM;
        constexpr static bool   secure  = true;

        toggle tcp_nodelay = false;
        toggle tcp_quickack = false;

        ssl_socket()
        {
            tcp_nodelay.on_toggle(nullptr);
            tcp_quickack.on_toggle(nullptr);
        }
    };
    #endif

    template <typename Socket>
    class socket : public Socket
    {
        constexpr static int default_backlog_size = 20;
        constexpr static int recv_buffer_size = 1024;

        public:
            using listen_callback_function_type = int;
            using recv_callback_function_type = int;

            static inline size_t mtu_size = 1200;

            constexpr static int uninitialised  = 0;
            constexpr static int disabled       = -2;

            socket() noexcept;
            socket(int in_socket_fd, int protocol) noexcept;

            socket(socket&& other) noexcept;
            socket& operator=(socket&& other) noexcept;

            socket(const socket& other) = delete;
           ~socket();

            // connecting
            error_code open(const char* host, uint16_t port) noexcept;
            error_code connect(const char* host, uint16_t port) noexcept;

            // TCP-specific
            error_code listen(uint16_t port, int backlog_size = 20) noexcept; // TODO: requires Socket::type == SOCK_STREAM
            template <typename Output>
            error_code accept(Output& target, std::chrono::milliseconds timeout = std::chrono::milliseconds(disabled)) noexcept;

            // cleanup
            void close() noexcept;

            // communicating
            ssize_t send(const buffer& data) const noexcept;
            buffer recv() noexcept;

            #if defined(HAS_CPPEVENTS)
            explicit socket(const cppevents::network_event&) noexcept;
            void connect_events(cppevents::event_queue& queue, int state);

            static socket& get_socket_from_event(const cppevents::network_event&) noexcept;
            #endif

        private:
            int get_socket(const char* host, uint16_t port, int family) noexcept;

            int socket_ipv6 = uninitialised;
            int socket_ipv4 = uninitialised;

            // platform-specific listening sockets
            #if defined(__linux__) || defined(__linux)
            int listen_fd = uninitialised;
            #endif

            #if defined(HAS_CPPEVENTS)
            mutable std::unordered_set<cppevents::event_queue*> linked_queues;
            static inline std::unordered_map<int, socket*> followed_sockets;
            #endif
    };

    template <typename SocketType>
    socket<SocketType>::socket() noexcept
    {
        // openssl stuff

        // setup toggles
        if constexpr(SocketType::type == SOCK_STREAM)
        {
            this->tcp_nodelay.on_toggle([&](bool& state, bool requested_state) {
                int flag = requested_state ? 1 : 0;
                if (socket_ipv4 > 0)
                {
                    int result = setsockopt(socket_ipv4,
                                            IPPROTO_TCP,
                                            TCP_NODELAY,
                                            (char*)&flag,
                                            sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
                if (socket_ipv6 > 0)
                {
                    int result = setsockopt(socket_ipv6,
                                            IPPROTO_TCP,
                                            TCP_NODELAY,
                                            (char*)&flag,
                                            sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
            });
            this->tcp_quickack.on_toggle([&](bool& state, bool requested_state) {
                int flag = requested_state ? 1 : 0;
                if (socket_ipv4 > 0)
                {
                    int result = setsockopt(socket_ipv4,
                                            IPPROTO_TCP,
                                            TCP_QUICKACK,
                                            (char*)&flag,
                                            sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
                if (socket_ipv6 > 0)
                {
                    int result = setsockopt(socket_ipv6,
                                            IPPROTO_TCP,
                                            TCP_QUICKACK,
                                            (char*)&flag,
                                            sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
            });
        } // if SOCK_STREAM == TCP
    }

    template <typename SocketType>
    void socket<SocketType>::close() noexcept
    {
        #if defined(HAS_CPPEVENTS)
        for (auto& queue : linked_queues)
        {
            queue->remove_native_source(socket_ipv6);
            queue->remove_native_source(socket_ipv4);
            queue->remove_native_source(listen_fd);
        }
        followed_sockets.erase(socket_ipv6);
        followed_sockets.erase(socket_ipv4);
        followed_sockets.erase(listen_fd);
        #endif
        if (socket_ipv6 > 0)
            ::close(socket_ipv6);
        if (socket_ipv4 > 0)
            ::close(socket_ipv4);
        #if defined(__linux__) || defined(__linux)
        if (listen_fd > 0)
            ::close(listen_fd);
        #endif
    }

    template <typename SocketType>
    socket<SocketType>::~socket()
    {
        close();
    }

    template <typename SocketType>
    socket<SocketType>::socket(socket&& other) noexcept
    {
        *this = std::move(other);
    }

    template <typename SocketType>
    socket<SocketType>& socket<SocketType>::operator=(socket&& other) noexcept
    {
        socket_ipv6 = other.socket_ipv6;
        socket_ipv4 = other.socket_ipv4;

        other.socket_ipv6 = other.socket_ipv6 == disabled ? disabled : uninitialised;
        other.socket_ipv4 = other.socket_ipv4 == disabled ? disabled : uninitialised;

        #if defined(__linux__) || defined(__linux)
        listen_fd = other.listen_fd;
        other.listen_fd = uninitialised;
        #endif

        #if defined(HAS_CPPEVENTS)
        if (followed_sockets.count(socket_ipv4))
            followed_sockets[socket_ipv4] = this;
        if (followed_sockets.count(socket_ipv6))
            followed_sockets[socket_ipv6] = this;
        #endif
        return *this;
    };

    template <typename SocketType>
    int socket<SocketType>::get_socket(const char* host, uint16_t port, int family) noexcept
    {
        addrinfo    hints;
        addrinfo*   server_info = nullptr;
        addrinfo*   info = nullptr;

        const int yes = 1;
        memset(&hints, 0, sizeof(hints));

        char portstr[6]; sprintf(portstr, "%d", port);

        hints.ai_family = family;
        hints.ai_socktype = SocketType::type;

        if (host == nullptr)
            hints.ai_flags = AI_PASSIVE;

        if (getaddrinfo(host, portstr, &hints, &server_info) != 0)
            return disabled;

        int socket_fd;

        for (info = server_info; info != nullptr; info = info->ai_next)
        {
            // Open socket
            if ((socket_fd = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1)
                continue;

            // Setup
            if (family == AF_INET6)
            {
                if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(int)) == -1)
                {
                    freeaddrinfo(server_info);
                    return error::setsockopt_failed;
                }
            }

            if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            {
                freeaddrinfo(server_info);
                return error::setsockopt_failed;
            }
/*
   TODO: way to configure this (and SO_REUSEADDR)
            if (config.reuse_port)
            {
                if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(int)) == -1)
                {
                    freeaddrinfo(server_info);
                    return error::setsockopt_failed;
                }
            }
*/
            // Bind / Connect
            if (host == nullptr)
            {
                if (::bind(socket_fd, info->ai_addr, info->ai_addrlen) == -1)
                {
                    ::close(socket_fd);
                    continue;
                }
            } else {
                if (::connect(socket_fd, info->ai_addr, info->ai_addrlen) == -1)
                {
                    ::close(socket_fd);
                    continue;
                }
            }
            break;
        }

        if (info == nullptr)
        {
            freeaddrinfo(server_info);
            return disabled;
        }

        freeaddrinfo(server_info);

        // FIXME: configure_socket(socket_fd);
        return socket_fd;
    }

    template <typename SocketType>
    socket<SocketType>::socket(int in_socket_fd, int protocol) noexcept
    {
        if (protocol == AF_INET)
        {
            socket_ipv4 = in_socket_fd;
            socket_ipv6 = disabled;
        }
        else if (protocol == AF_INET6)
        {
            socket_ipv6 = in_socket_fd;
            socket_ipv4 = disabled;
        }
        else
        {
            socket_ipv6 = disabled;
            socket_ipv4 = disabled;
        }
        //socket();
    }

    template <typename SocketType>
    error_code socket<SocketType>::open(const char* host, uint16_t port) noexcept
    {
        if ((socket_ipv6 > 0) || (socket_ipv4 > 0))
            return error::socket_already_open;

        if (host == nullptr)
        {
            socket_ipv6 = get_socket(host, port, AF_INET6);
            socket_ipv4 = get_socket(host, port, AF_INET);
        } else {
            socket_ipv6 = get_socket(host, port, AF_INET6);
            if (socket_ipv6 > 0)
                socket_ipv4 = disabled;
            else
                socket_ipv4 = get_socket(host, port, AF_INET);
        }

        if ((socket_ipv6 >= 0) || (socket_ipv4 >= 0))
            return 0;

        return error::cannot_open_socket;
    }

    template <typename SocketType>
    error_code socket<SocketType>::connect(const char* host, uint16_t port) noexcept
    {
        return open(host, port);
    }

    template <typename SocketType>
    error_code socket<SocketType>::listen(uint16_t port, int backlog_size) noexcept
    {
        static_assert(SocketType::type == SOCK_STREAM, "Cannot use listen() with a datagram socket");
        if (listen_fd > 0)
            ::close(listen_fd);

        #if defined(__linux__) || defined(__linux)
        listen_fd = epoll_create1(0);

        error_code status = open(nullptr, port);
        if (status)
            return status;

        if (socket_ipv6 > 0)
        {
            if (::listen(socket_ipv6, backlog_size) == -1)
            {
                ::close(socket_ipv6);
                socket_ipv6 = disabled;

                return error::could_not_listen;
            }

            epoll_event event;
            memset(&event, 0, sizeof(event));
            event.events = EPOLLIN;
            event.data.fd = socket_ipv6;

            if (epoll_ctl(listen_fd, EPOLL_CTL_ADD, socket_ipv6, &event))
            {
                ::close(socket_ipv6);
                socket_ipv6 = disabled;

                return error::epoll_error;
            }
        }
        if (socket_ipv4 > 0)
        {
            if (::listen(socket_ipv4, backlog_size) == -1)
            {
                ::close(socket_ipv4);
                socket_ipv4 = disabled;

                return error::could_not_listen;
            }

            epoll_event event;
            memset(&event, 0, sizeof(event));
            event.events = EPOLLIN;
            event.data.fd = socket_ipv4;

            if (epoll_ctl(listen_fd, EPOLL_CTL_ADD, socket_ipv4, &event))
            {
                ::close(socket_ipv4);
                socket_ipv4 = disabled;

                return error::epoll_error;
            }
        }
        return 0;
        #else
        return error::unimplemented_feature;
        #endif
    }

    template <typename SocketType> template <typename Output>
    error_code socket<SocketType>::accept(Output& out_target, std::chrono::milliseconds timeout) noexcept
    {
        static_assert(is_stl_like_container<Output>() || std::is_same<Output&, socket<SocketType>&>::value,
                      "Output parameter needs to be either socket reference or stl-compatible array");
        static_assert(SocketType::type == SOCK_STREAM,
                      "Cannot use accept() with a datagram socket");

        if ((socket_ipv6 <= 0) && (socket_ipv4 <= 0))
            return error::no_active_socket;

        sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);

        #if defined(__linux__) || defined(__linux)
        int event_count = 0;
        int max_events = 0;
        epoll_event* target = nullptr;
        if constexpr (is_stl_like_container<Output>())
        {
            static_assert(std::is_same<typename Output::value_type, socket<SocketType>>::value);
            epoll_event events[EPOLL_MAX_EVENTS];
            target = events;
            max_events = EPOLL_MAX_EVENTS;
        } else {
            epoll_event event;
            target = &event;
            max_events = 1;
        }

        if (timeout.count() < 0)
            event_count = epoll_wait(listen_fd, target, max_events, -1);
        else 
            event_count = epoll_wait(listen_fd, target, max_events, timeout.count());

        for (int i = 0; i < event_count; ++i)
        {
            int new_fd = ::accept((target + i)->data.fd, (sockaddr*)&their_addr, &addr_size);
            if (new_fd == -1)
                continue;
            char s[INET6_ADDRSTRLEN];
            inet_ntop(their_addr.ss_family,
                      helpers::get_in_addr((sockaddr*)&their_addr),
                      s, sizeof(s));
            int ctype = -1;
            if ((target + i)->data.fd == socket_ipv6)
                ctype = AF_INET6;
            else if ((target + i)->data.fd == socket_ipv4)
                ctype = AF_INET;
            else 
                ; // No ctype?

            if constexpr (is_stl_like_container<Output>())
            {
                out_target.emplace_back(socket(new_fd, ctype));
            } else {
                out_target = socket(new_fd, ctype);
            }
        }

        return 0;
        #endif
    }

    template <typename SocketType>
    ssize_t socket<SocketType>::send(const buffer& data) const noexcept
    {
        if ((socket_ipv4 <= 0) && (socket_ipv6 <= 0))
            return error::no_active_socket;

        int socket_fd = socket_ipv4 == disabled ? socket_ipv6 : socket_ipv4;

        int sent = 0;
        int left = data.size();
        int n = 0;
        while (sent < static_cast<int>(data.size()))
        {
            n = ::send(socket_fd, data.data() + sent, left, 0);
            if (n == -1)
            {
                std::cout << strerror(errno) << "\n"; 
                return error::failed_to_send_data;
            }

            sent += n;
            left -= n;
        }
        return sent;
    }

    template <typename SocketType>
    buffer socket<SocketType>::recv() noexcept
    {
        constexpr static int no_flags = 0;

        buffer rval;
        if ((socket_ipv4 <= 0) && (socket_ipv6 <= 0))
            return rval;

        int socket_fd = socket_ipv4 == disabled ? socket_ipv6 : socket_ipv4;
        char chunk[recv_buffer_size];
        memset(chunk, 0, recv_buffer_size);

        ssize_t bytes = ::recv(socket_fd, chunk, recv_buffer_size, no_flags);
        if (bytes < 0)
            return rval;

        if (bytes == 0)
        {
            // remote closed connection
            close();
            return rval;
        }

        rval += chunk;

        if (bytes < recv_buffer_size)
            return rval;

        while(true)
        {
            memset(chunk, 0, recv_buffer_size);
            bytes = ::recv(socket_fd, chunk, recv_buffer_size, no_flags | MSG_DONTWAIT);
            if (bytes == 0)
                break;
            else if (bytes < 0)
            {
            }
            rval += chunk;
        }
        return rval;
    }

    using tcp_socket = unorthodox::socket<tcp_socket_details>;
    using udp_socket = unorthodox::socket<udp_socket_details>;
    #if defined(UNORTHODOX_SSL_PROVIDER)
    using ssl_socket = unorthodox::socket<ssl_socket_details>;
    #endif
}

#if defined(HAS_CPPEVENTS)
constexpr static int fd_type_listening_socket = 0;
constexpr static int fd_type_io_socket = 1;

namespace unorthodox::detail
{
    inline cppevents::event create_socket_listen_event(int fd)
    {
        cppevents::network_event ev;

        sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);

        int actual_socket_fd = -1;

        #if defined(__linux__) || defined(__linux)
        epoll_event event;
        epoll_wait(fd, &event, 1, -1);
        actual_socket_fd = event.data.fd;
        #endif

        int new_fd = ::accept(actual_socket_fd, (sockaddr*)&their_addr, &addr_size);
        if (new_fd == -1)
        {
            std::cout << strerror(errno) << "\n";
            return ev;
        }

        char s[INET6_ADDRSTRLEN];
        inet_ntop(their_addr.ss_family,
                  helpers::get_in_addr((sockaddr*)&their_addr),
                  s, sizeof(s));

        std::cout << "new: " << new_fd << "\n";
        ev.type = cppevents::network_event::new_connection;
        ev.peer_address = s;
        ev.sock_handle = new_fd;

        return std::move(ev);
    }

    inline cppevents::event create_socket_io_event(int fd)
    {
        cppevents::network_event ev;
        ev.type = cppevents::network_event::socket_ready;
        ev.sock_handle = fd;
        return std::move(ev);
    }
}
namespace unorthodox
{
    template <typename SocketType>
    socket<SocketType>::socket(const cppevents::network_event& ev) noexcept
        : socket(ev.sock_handle, helpers::deduce_protocol_from_address(ev.peer_address))
    {}

    template <typename SocketType>
    inline void socket<SocketType>::connect_events(cppevents::event_queue& queue, int state)
    {
        linked_queues.emplace(&queue);
        if (state == fd_type_listening_socket)
            queue.add_native_source(listen_fd, detail::create_socket_listen_event);
        else
        {
            if (socket_ipv4 > 0)
            {
                queue.add_native_source(socket_ipv4, detail::create_socket_io_event);
                followed_sockets[socket_ipv4] = this;
            }
            if (socket_ipv6 > 0)
            {
                queue.add_native_source(socket_ipv6, detail::create_socket_io_event);
                followed_sockets[socket_ipv6] = this;
            }
        }
    }
    
    template <typename SocketType>
    socket<SocketType>& socket<SocketType>::get_socket_from_event(const cppevents::network_event& ev) noexcept
    {
        assert(socket<SocketType>::followed_sockets.count(ev.sock_handle));
        assert(socket<SocketType>::followed_sockets[ev.sock_handle] != nullptr);
        return *socket<SocketType>::followed_sockets[ev.sock_handle];
    }
}
// Specialise cppevents templates
namespace cppevents
{
    template <> error_code add_source<cppevents::socket_listener, unorthodox::tcp_socket>(
        unorthodox::tcp_socket& socket,
        event_queue& queue)
    {
        socket.connect_events(queue, fd_type_listening_socket);

        return error_code::success;
    }

    template <> error_code add_source<cppevents::socket_io_event, unorthodox::tcp_socket>(
        unorthodox::tcp_socket& socket,
        event_queue& queue)
    {
        socket.connect_events(queue, fd_type_io_socket);

        return error_code::success;
    }
}
#endif

#endif
