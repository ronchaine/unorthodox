#ifndef UNORTHODOX_NETWORK_SOCKETS_HPP
#define UNORTHODOX_NETWORK_SOCKETS_HPP

#include <unorthodox/buffer.hpp>
#include <unorthodox/extra_type_traits.hpp>
#include <unorthodox/error_codes.hpp>

#include <functional>
#include <chrono>

#if defined(HAS_CPPEVENTS)
#include <cppevents/network.hpp>
#endif

#if defined(__linux__) || defined(__linux)
# define UNORTHODOX_OS_LINUX
# define QUEUE_MAX_EVENTS 32
# include <sys/socket.h>
# include "sockets_os_posix.hpp"
#elif defined(__bsdi__) || defined(__DragonFly__) \
    || defined(__FreeBSD__) || defined(__FreeBSD_kernel__) \
    || defined(__OpenBSD__) || defined(__NetBSD__)
# define UNORTHODOX_OS_BSD
# define QUEUE_MAX_EVENTS 32
# include <sys/event.h>
# include <sys/socket.h>
# warning BSD support not done
#elif (defined(_WIN32))
# include "sockets_os_win32.hpp"
# warning WIN32 support not done
#elif (defined(__XTENSA__) && defined(WITH_POSIX))
# if TARGET == ESP32
# error No Xtensa implementation (yet)
# define UNORTHODOX_OS_RTOS_POSIX
# else
# error Only EPS32 support for Xtensa is planned
# endif
#else
#error Unrecognised target OS / architecture
#endif

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <cstring>

#if defined(HAS_CPPEVENTS)
namespace unorthodox::net::detail
{
    inline cppevents::event create_socket_listen_event(os::socket_type fd);
    inline cppevents::event create_socket_io_event(os::socket_type fd);
    inline cppevents::network_event create_disconnect_event(os::socket_type fd);
}
#endif

namespace unorthodox::net
{
    namespace helpers
    {
        inline int deduce_protocol_from_address(const char* s)
        {
            addrinfo* result;
            getaddrinfo(s, nullptr, nullptr, &result);
            int rval = result->ai_family;
            freeaddrinfo(result);
            return rval;
        }

        inline int deduce_protocol_from_address(const std::string& s)
        {
            return deduce_protocol_from_address(s.c_str());
        }

        inline void* get_in_addr(struct sockaddr* sa)
        {
            if (sa->sa_family == AF_INET)
                return &((reinterpret_cast<struct sockaddr_in*>(sa))->sin_addr);
            else
                return &((reinterpret_cast<struct sockaddr_in6*>(sa))->sin6_addr);
        }
    }

    class toggle
    {
        public:
            using toggle_function_type = std::function<void(bool&, bool)>;

            toggle() = default;
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

    struct tcp_socket_details
    {
        constexpr static int    domain  = PF_INET;
        constexpr static int    type    = SOCK_STREAM;
        constexpr static bool   secure  = false;

        toggle tcp_nodelay;
        toggle tcp_quickack;
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
    };
    #endif

    #if defined(HAS_CPPEVENTS)
    enum class fd_role {
        listening_socket,
        io_socket,
        unknown,
    };
    #endif

    template <typename Socket, bool Owning = true>
    class socket : public Socket, os::socket_specifics
    {
        constexpr static int default_backlog_size = 20;
        constexpr static int recv_buffer_size = 1024;

        public:
            constexpr static os::socket_type uninitialised  = os::uninitialised_socket_value;
            constexpr static os::socket_type disabled       = os::disabled_socket_value;

            constexpr static bool is_secure = Socket::secure;

            size_t mtu_size = 1200;

            socket() noexcept requires (Owning == true);
            socket(os::socket_type in_socket_fd, int protocol) noexcept;

            socket(socket&& other) noexcept;
            socket& operator=(socket&& other) noexcept;

            socket(const socket& other) = delete;
           ~socket();

            // connecting
            error_code open(const char* host, uint16_t port) noexcept;
            error_code connect(const char* host, uint16_t port) noexcept;

            // Stream-specific
            error_code listen(uint16_t port, int backlog_size = default_backlog_size) noexcept requires (Socket::type == SOCK_STREAM);

            template <typename Output> requires (Socket::type == SOCK_STREAM)
            error_code accept(Output& target, std::chrono::milliseconds timeout = std::chrono::milliseconds(-1)) noexcept;

            // cleanup
            void close() noexcept;

            // communicating
            template <typename T> requires stl_compatible_container<T>
            tl::expected<size_t, error_code> send(const T& data) const noexcept;

            template <typename T>
            tl::expected<size_t, error_code> send(const T& data) const noexcept;

            template <typename T> requires stl_compatible_container<T>
            tl::expected<T, error_code> recv() noexcept;

            #if defined(HAS_CPPEVENTS)
            constexpr static bool cppevent_unorthodox_socket = true;

            explicit socket(const cppevents::network_event&) noexcept;
            void connect_events(cppevents::event_queue& queue, fd_role state);

            static socket<Socket, false> get_socket_from_event(const cppevents::network_event&) noexcept;
            #endif

        private:
            tl::expected<os::socket_type, error_code> get_socket(const char* host, uint16_t port, int family) noexcept;

            os::socket_type socket_ipv6 = uninitialised;
            os::socket_type socket_ipv4 = uninitialised;
    };

    using udp_socket = unorthodox::net::socket<udp_socket_details>;
    using tcp_socket = unorthodox::net::socket<tcp_socket_details>;
    
    using udp_socket_view = unorthodox::net::socket<udp_socket_details, false>;
    using tcp_socket_view = unorthodox::net::socket<tcp_socket_details, false>;

    #if defined(UNORTHODOX_SSL_PROVIDER)
    using ssl_socket = unorthodox::net::socket<ssl_socket_details>;
    #endif
}

namespace unorthodox::net {
    template <typename SocketType, bool Own>
    socket<SocketType, Own>::socket() noexcept requires (Own == true)
    {
        // openssl stuff should be handled by ssl_socket_details

        // setup toggles for stream sockets
        if constexpr(SocketType::type == SOCK_STREAM)
        {
            this->tcp_nodelay.on_toggle([&](bool& state, bool requested_state) {
                int flag = requested_state ? 1 : 0;
                if (os::is_active_socket(socket_ipv4))
                {
                    int result = setsockopt(socket_ipv4, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&flag), sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
                if (os::is_active_socket(socket_ipv6))
                {
                    int result = setsockopt(socket_ipv6, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&flag), sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
            });

            this->tcp_quickack.on_toggle([&](bool& state, bool requested_state) {
                int flag = requested_state ? 1 : 0;
                if (os::is_active_socket(socket_ipv4))
                {
                    int result = setsockopt(socket_ipv4, IPPROTO_TCP, TCP_QUICKACK, reinterpret_cast<char*>(&flag), sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
                if (os::is_active_socket(socket_ipv6))
                {
                    int result = setsockopt(socket_ipv6, IPPROTO_TCP, TCP_QUICKACK, reinterpret_cast<char*>(&flag), sizeof(int));
                    if (result >= 0)
                        state = requested_state;
                }
            });
        }
    }

    template <typename SocketType, bool Owner>
    void socket<SocketType, Owner>::close() noexcept
    {
        if (os::is_active_socket(socket_ipv6))
            ::close(socket_ipv6);
        if (os::is_active_socket(socket_ipv4))
            ::close(socket_ipv4);

        stop_listening();
    }

    template <typename SocketType, bool Owner>
    socket<SocketType, Owner>::~socket()
    {
        if constexpr(Owner == true)
            close();
    }

    template <typename SocketType, bool Owner>
    socket<SocketType, Owner>::socket(socket&& other) noexcept : os::socket_specifics(std::move(other))
    {
        *this = std::move(other);
    }

    template <typename SocketType, bool Owner>
    socket<SocketType, Owner>& socket<SocketType, Owner>::operator=(socket&& other) noexcept
    {
        if (this == &other)
            return *this;

        close();

        socket_ipv6 = other.socket_ipv6;
        socket_ipv4 = other.socket_ipv4;

        other.socket_ipv6 = other.socket_ipv6 == disabled ? disabled : uninitialised;
        other.socket_ipv4 = other.socket_ipv4 == disabled ? disabled : uninitialised;

        other.socket_ipv4 = other.socket_ipv4 == disabled ? disabled : uninitialised;

        this->listen_fd = other.listen_fd;
        other.listen_fd = other.listen_fd == disabled ? disabled : uninitialised;

        return *this;
    }

    template <typename SocketType, bool Owning>
    tl::expected<os::socket_type, error_code> socket<SocketType, Owning>::get_socket(const char* host, uint16_t port, int family) noexcept
    {
        addrinfo    hints{};
        addrinfo*   server_info = nullptr;
        addrinfo*   info = nullptr;

        const int yes = 1;

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
            if ((socket_fd = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == os::invalid_socket)
                continue;

            if (family == AF_INET6)
            {
                if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(int)) == os::invalid_socket)
                {
                    freeaddrinfo(server_info);
                    return tl::unexpected(error_code(error_domain::network_error, error_code::setsockopt_failed));
                }
            }
            if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == os::invalid_socket)
            {
                freeaddrinfo(server_info);
                return tl::unexpected(error_code(error_domain::network_error, error_code::setsockopt_failed));
            }

            if (host == nullptr)
            {
                if (::bind(socket_fd, info->ai_addr, info->ai_addrlen) == os::invalid_socket)
                {
                    ::close(socket_fd);
                    continue;
                }
            } else {
                if (::connect(socket_fd, info->ai_addr, info->ai_addrlen) == os::invalid_socket)
                {
                    ::close(socket_fd);
                    continue;
                }
            }
            break;
        }

        freeaddrinfo(server_info);
        if (info == nullptr)
            return disabled;

        return socket_fd;
    }

    template <typename SocketType, bool Owner>
    socket<SocketType, Owner>::socket(os::socket_type in_socket_fd, int protocol) noexcept
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

        // TODO: Init SSL here
    }

    template <typename SocketType, bool Owner>
    error_code socket<SocketType, Owner>::open(const char* host, uint16_t port) noexcept
    {
        if (os::is_active_socket(socket_ipv6) || os::is_active_socket(socket_ipv4))
            return error_code(error_domain::network_error, error_value::socket_already_open);

        if (host == nullptr)
        {
            socket_ipv6 = get_socket(host, port, AF_INET6).value_or(disabled);
            socket_ipv4 = get_socket(host, port, AF_INET).value_or(disabled);
        } else {
            socket_ipv6 = get_socket(host, port, AF_INET6).value_or(disabled);
            if (socket_ipv6 == disabled)
                socket_ipv4 = get_socket(host, port, AF_INET).value_or(disabled);
        }

        if (os::is_active_socket(socket_ipv6) || os::is_active_socket(socket_ipv4))
            return error_code::success;

        return error_code(error_domain::network_error, error_value::cannot_open_socket);
    }

    template <typename SocketType, bool Owner>
    error_code socket<SocketType, Owner>::connect(const char* host, uint16_t port) noexcept
    {
        return open(host, port);
    }

    template <typename SocketType, bool Owner>
    error_code socket<SocketType, Owner>::listen(uint16_t port, int backlog_size) noexcept requires (SocketType::type == SOCK_STREAM)
    {
        error_code status = open(nullptr, port);
        if (status) return status;

        if (os::is_active_socket(socket_ipv4))
        {
            status = platform_listen_socket(port, backlog_size, socket_ipv4, AF_INET);
            if (status.code == error_value::could_not_listen || status.code == error_value::poll_error)
            {
                ::close(socket_ipv4);
                socket_ipv4 = disabled;
            }
            if (status)
                return status;
        }

        if (os::is_active_socket(socket_ipv6))
        {
            status = platform_listen_socket(port, backlog_size, socket_ipv6, AF_INET6);
            if (status.code == error_value::could_not_listen || status.code == error_value::poll_error)
            {
                ::close(socket_ipv6);
                socket_ipv6 = disabled;
            }
            if (status)
                return status;
        }

        return status;
    }

    template <typename SocketType, bool Owner> template <typename Output> requires (SocketType::type == SOCK_STREAM)
    error_code socket<SocketType, Owner>::accept(Output& out_target, std::chrono::milliseconds timeout) noexcept
    {
        static_assert(is_stl_like_container<Output>() || std::is_same<Output&, socket<SocketType>&>::value,
                "Output parameter needs to be either socket reference or stl-compatible array of such");

        if (!os::is_active_socket(socket_ipv6) && !os::is_active_socket(socket_ipv4))
            return error_code(error_domain::network_error, error_value::no_active_socket);

        sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);

        int event_count = 0;
        int max_events = 0;

        platform_event_type* target = nullptr;
        platform_event_type events[QUEUE_MAX_EVENTS];

        if constexpr (is_stl_like_container<Output>())
        {
            static_assert(std::is_same<typename Output::value_type, socket<SocketType>>::value);
            target = events;
            max_events = QUEUE_MAX_EVENTS;
        } else {
            platform_event_type event;
            target = &event;
            max_events = 1;
        }

        event_count = block_until_listen_event(target, max_events, timeout);

        for (int i = 0; i < event_count; ++i)
        {
            os::socket_type new_socket = ::accept(platform_get_socket_from_event(*(target + i)),
                                                  reinterpret_cast<sockaddr*>(&their_addr),
                                                  &addr_size);

            if (new_socket == os::invalid_socket)
               continue;

            char s[INET6_ADDRSTRLEN];
            inet_ntop(their_addr.ss_family,
                      helpers::get_in_addr(reinterpret_cast<sockaddr*>(&their_addr)),
                      s, sizeof(s));

            int af_type = platform_get_socket_from_event(*(target + i)) == socket_ipv4 ? AF_INET : AF_INET6;

            if constexpr(is_stl_like_container<Output>())
               out_target.emplace_back(socket(new_socket, af_type));
            else 
               out_target = socket(new_socket, af_type);
        }
        return 0;
    }

    template <typename SocketType, bool Owner> template <typename T> requires stl_compatible_container<T>
    tl::expected<size_t, error_code> socket<SocketType, Owner>::send(const T& data) const noexcept
    {
        if (!os::is_active_socket(socket_ipv6) && !os::is_active_socket(socket_ipv4))
            return error_code(error_domain::network_error, error_value::no_active_socket);

        const int socket_fd = socket_ipv4 == disabled ? socket_ipv6 : socket_ipv4;

        int sent = 0;
        int left = data.size();

        int n = 0;

        while (sent < static_cast<int>(data.size()))
        {
           n = ::send(socket_fd, data.data() + sent, left, 0);
           if (n == -1)
              return error_code(error_domain::network_error, error_value::failed_to_send_data);

           sent += n;
           left -= n;
        }

        return sent;
    }

    // this probably requires a small refactor with the sent < total - duplication...
    template <typename SocketType, bool Owner> template <typename T>
    tl::expected<size_t, error_code> socket<SocketType, Owner>::send(const T& data) const noexcept
    {
        std::cout << "socket status, ipv4: " << socket_ipv4 << ", ipv6: " << socket_ipv6 << "\n";

        if (!os::is_active_socket(socket_ipv6) && !os::is_active_socket(socket_ipv4))
            return error_code(error_domain::network_error, error_value::no_active_socket);

        constexpr static int extent = std::extent_v<T>;

        const int socket_fd = socket_ipv4 == disabled ? socket_ipv6 : socket_ipv4;

        std::cout << "sending to socket " << socket_fd << "\n";

        int sent = 0;
        int n = 0;

        if constexpr(extent == 0)
        {
            const int total = sizeof(T);
            int left = total;

            while (sent < total)
            {
                n = ::send(socket_fd, &data, left, 0);
                if (n == -1)
                    return error_code(error_domain::network_error, error_value::failed_to_send_data);

                sent += n;
                left -= n;
            }
        }
        else // we are dealing with an array of some kind
        {
            const int total = extent * sizeof(decltype(data[0]));
            int left = total;

            while (sent < total)
            {
                n = ::send(socket_fd, &data[sent], left, 0);
                if (n == -1)
                    return error_code(error_domain::network_error, error_value::failed_to_send_data);

                sent += n;
                left -= n;
            }
        }
        return sent;
    }

    // UNSTABLE
    template <typename SocketType, bool Owner> template <typename T> requires stl_compatible_container<T>
    tl::expected<T, error_code> socket<SocketType, Owner>::recv() noexcept
    {
        constexpr static int no_flags = 0;

        T rval{};
        if (!os::is_active_socket(socket_ipv6) && !os::is_active_socket(socket_ipv4))
            return tl::unexpected(error_code(error_domain::network_error, error_value::no_active_socket));

        const int socket_fd = socket_ipv4 == disabled ? socket_ipv6 : socket_ipv4;
        std::array<std::byte, recv_buffer_size> chunk;

        size_t total_recv = 0;

        bool multiple_chunks = false;
        while(true)
        {
            chunk.fill(std::byte(0));
            ssize_t bytes = ::recv(socket_fd, chunk.data(), recv_buffer_size, multiple_chunks ? no_flags | MSG_DONTWAIT : no_flags);
            if (bytes == 0)
            {
                close();
                break;
            }
            else if (bytes < 0)
            {
                return tl::unexpected(error_code(error_domain::network_error, error_value::from_errno()));
            }

            rval.resize(total_recv + bytes);
            std::memmove(rval.data() + total_recv, chunk.data(), bytes);
            total_recv += bytes;

            // did we read all there was?
            if (bytes < recv_buffer_size)
                break;

            multiple_chunks = true;
        }

        return rval;
    }
}

#if defined(HAS_CPPEVENTS)
namespace unorthodox::net
{
    namespace detail
    {
        inline cppevents::event create_socket_listen_event(os::socket_type fd)
        {
            // we do not need this
            (void) fd;

            cppevents::network_event ev;

            sockaddr_storage their_addr;
            socklen_t addr_size = sizeof(their_addr);

            os::socket_type actual_socket_fd = os::invalid_socket;

            // LINUX-ONLY, move to sockets_os_posix.hpp
            epoll_event event;
            epoll_wait(fd, &event, 1, -1);
            actual_socket_fd = event.data.fd;

            os::socket_type new_fd = ::accept(actual_socket_fd, reinterpret_cast<sockaddr*>(&their_addr), &addr_size);
            if (new_fd == os::invalid_socket)
                return ev;

            char s[INET6_ADDRSTRLEN];
            inet_ntop(their_addr.ss_family,
                      helpers::get_in_addr(reinterpret_cast<sockaddr*>(&their_addr)),
                      s, sizeof(s));

            ev.type = cppevents::network_event::new_connection;
            ev.peer_address = s;
            ev.sock_handle = new_fd;

            if (their_addr.ss_family == AF_INET)
                ev.peer_port = ntohs(reinterpret_cast<sockaddr_in*>(&their_addr)->sin_port);
            else if (their_addr.ss_family == AF_INET6)
                ev.peer_port = ntohs(reinterpret_cast<sockaddr_in6*>(&their_addr)->sin6_port);

            return std::move(ev);
        }

        inline cppevents::event create_socket_io_event(os::socket_type fd)
        {
            cppevents::network_event ev;
            ev.type = cppevents::network_event::socket_ready;
            ev.sock_handle = fd;

            sockaddr_storage their_addr;
            socklen_t addr_size = sizeof(their_addr);

            getpeername(fd, reinterpret_cast<sockaddr*>(&their_addr), &addr_size);
            char s[INET6_ADDRSTRLEN];
            inet_ntop(their_addr.ss_family,
                      helpers::get_in_addr(reinterpret_cast<sockaddr*>(&their_addr)),
                      s, sizeof(s));

            ev.peer_address = s;
            if (their_addr.ss_family == AF_INET)
                ev.peer_port = ntohs(reinterpret_cast<sockaddr_in*>(&their_addr)->sin_port);
            else if (their_addr.ss_family == AF_INET6)
                ev.peer_port = ntohs(reinterpret_cast<sockaddr_in6*>(&their_addr)->sin6_port);

            return std::move(ev);
        }

        inline cppevents::network_event create_disconnect_event(os::socket_type fd)
        {
            cppevents::network_event ev;
            ev.type = cppevents::network_event::connection_closed;
            ev.sock_handle = fd;

            sockaddr_storage their_addr;
            socklen_t addr_size = sizeof(their_addr);

            getpeername(fd, reinterpret_cast<sockaddr*>(&their_addr), &addr_size);
            char s[INET6_ADDRSTRLEN];
            inet_ntop(their_addr.ss_family,
                      helpers::get_in_addr(reinterpret_cast<sockaddr*>(&their_addr)),
                      s, sizeof(s));

            ev.peer_address = s;

            if (their_addr.ss_family == AF_INET)
                ev.peer_port = ntohs(reinterpret_cast<sockaddr_in*>(&their_addr)->sin_port);
            else if (their_addr.ss_family == AF_INET6)
                ev.peer_port = ntohs(reinterpret_cast<sockaddr_in6*>(&their_addr)->sin6_port);

            return ev;
        }
    }

    // cppevents-enabled socket constructor
    template <typename SocketType, bool Owner>
    socket<SocketType, Owner>::socket(const cppevents::network_event& ev) noexcept
        : socket(ev.sock_handle, helpers::deduce_protocol_from_address(ev.peer_address))
    {}

    template <typename SocketType, bool Owner>
    socket<SocketType, false> socket<SocketType, Owner>::get_socket_from_event(const cppevents::network_event& ev) noexcept
    {
        return socket<SocketType, false>(ev.sock_handle, helpers::deduce_protocol_from_address(ev.peer_address));
    }

    template <typename SocketType, bool Owner>
    void socket<SocketType, Owner>::connect_events(cppevents::event_queue& queue, fd_role state)
    {
        static_assert(Owner == true, "Currently cannot connect events to nonowning socket");
        switch(state)
        {
            case fd_role::listening_socket:
                queue.add_native_source(this->listen_fd, detail::create_socket_listen_event);
                break;
            case fd_role::io_socket:
                if (os::is_active_socket(socket_ipv4))
                    queue.add_native_source(socket_ipv4, detail::create_socket_io_event);
                if (os::is_active_socket(socket_ipv6))
                    queue.add_native_source(socket_ipv6, detail::create_socket_io_event);
                break;
            case fd_role::unknown:
                break;
        }
    }
}

namespace cppevents
{
    // specialised overloads for templates
    template <typename Source, typename Sock> 
        requires (std::is_same_v<typename Source::event_type, network_event> && Sock::cppevent_unorthodox_socket)
    error_code add_source(
        Sock& socket,
        event_queue& queue = default_queue)
    {
        if constexpr (std::is_same_v<Source, cppevents::socket_listener>)
            socket.connect_events(queue, unorthodox::net::fd_role::listening_socket);
        else if constexpr (std::is_same_v<Source, cppevents::socket_io_event>)
            socket.connect_events(queue, unorthodox::net::fd_role::io_socket);
        return error_code::success;
    }
}

#endif

#endif
