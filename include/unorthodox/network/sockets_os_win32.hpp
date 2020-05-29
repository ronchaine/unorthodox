#ifndef UNORTHODOX_NETWORK_SOCKETS_PLATFORM_HPP
#define UNORTHODOX_NETWORK_SOCKETS_PLATFORM_HPP

namespace unorthodox::network::os
{
    class socket_type
    {
        public:

        private:
            // for knowing when to call WSAStartup and WSACleanup
            inline static int socket_count = 0;

            SOCKET socket_ipv4 = INVALID_SOCKET;
            SOCKET socket_ipv6 = INVALID_SOCKET;
            SOCKET listen_sock = INVALID_SOCKET;

            struct status_flags
            {
                bool initialised : 1 = false;
                bool enabled : 1 = false;
                bool listening_ipv4 : 1 = false;
                bool listening_ipv6 : 1 = false;
            } status;
    };
}

#endif
